#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <endian.h>
#include "mapElements.h"
#include "dij.h"
#include "display.h"

#define FAILED_ROOM_ATTEMPTS_LIMIT 32

#define SECOND_CLOSEST_ROOM_CONNECTION_ODDS/*One in...*/ 3

#define FILE_PATH "/.rlg327/dungeon"
//"/.rlg327/jerBear/06.rlg327" //
FILE * fp;

//TODO make this a struct with some boolean flags
//TODO Strategy pattern by storing a function pointer.
enum IncomingCommand{
	nothing,
	save,
	load,
	load_save,
	distances
};

//TODO What a mess
int parseCLI(int argc, char * argv[], enum IncomingCommand * command, long * seed);

void generateNewMap(Map * map, long seed);
void initializeMap(Map *map);
void populateWithRooms(Map *map);
Room generateNewRoom(Map *map);
int isLegalRoom(Room *room, Map *map);
void placeNewRoom(Map *map, Room *room);

void populateWithCorridors(Map *map);
int dist(Room *r1, Room *r2);
Corridor * generateNewCorridor(Coordinate c1, Coordinate c2);
void placeNewCorridor(Corridor cor, Map *map);
void placePartialCorridor(Coordinate origin, int dist, bool horizontal, Map *map);
bool isSentinel(Room room);

void populateWithStairs(Map *map);
void assignTypeToRandomBlock(enum BlockType toPlace, enum BlockType canReplace[], int canReplaceSize, Map *map);

void populateWithPC(Map * map);

void printMap(Map * map);
void printRoomList(Room *roomList);
char getVisual(enum BlockType type);
void glog(char *string);

void testMapPrint(void);
bool isOnBorder(Coordinate point, Coordinate ul, Coordinate lr);

//-----------I/O--------------------
int openFile(char * mode);
int readFile(Map * newMap);
int closeFile(void);
int writeFile(Map * theMap);

//TODO Debug mode
int main(int argc, char *argv[]){

	long seed;
	enum IncomingCommand whatsup;
	if(parseCLI(argc,argv,&whatsup,&seed)!=0){
		printf("Incorrect usage\n");
		return -1;
	}
	Map theMap;
	//TODO generateNewMap also inits map. Which one should have that?
	//TODO Support custom seeds
	map_init(&theMap);
	switch(whatsup){
		case nothing:
		printf("Seed:%ld\n", seed);
		generateNewMap(&theMap,seed);
		printMap(&theMap);
		return 0;

		case load:
		readFile(&theMap);
		printMap(&theMap);
		return 0;

		case save:
		printf("Seed:%ld\n", seed);
		generateNewMap(&theMap,seed);
		printMap(&theMap);
		writeFile(&theMap);
		return 0;

		case load_save:
		readFile(&theMap);
		printMap(&theMap);
		writeFile(&theMap);
		return 0;

		case distances:
		printf("Seed:%ld\n", seed);
		generateNewMap(&theMap,seed);
		printMap(&theMap);
		DistanceMap dist;
		get_distance_map(&theMap,theMap.pcPos,&dist);
		display_distance_map(&dist);
		return 0;

		default:
		printf("That command is unfortunately not supported right now.\n");
		return 0;
	}
}


int parseCLI(int argc, char * argv[], enum IncomingCommand * command, long * seed){
	*seed = time(0);
	if(argc > 3){
		return -1;
	}
	if(argc == 3){
		*command = load_save;
	}else if(argc == 1){
		*command = nothing;
	}else if(!(strcmp(argv[1],"--load"))){
		*command = load;
	}else if(!(strcmp(argv[1],"--save"))){
		*command = save;
	}else if(!(strcmp(argv[1],"--distances"))){
		*command = distances;
	}else{
		return -2;
	}
	return 0;
}


void generateNewMap(Map * map, long seed){
	srand(seed);
	initializeMap(map);
	populateWithRooms(map);
	populateWithCorridors(map);
	populateWithStairs(map);
	populateWithPC(map);
}
void initializeMap(Map *map){
	// printf("Initializing map...\n");
	map_init(map);
	// printf("Map initialized\n");
}

//----------------------------ROOMS------------------------------
void populateWithRooms(Map *map){
	int roomQuota = (rand()%(MAX_ROOM_COUNT - MIN_ROOM_COUNT)) + MIN_ROOM_COUNT;
	int roomCount = 0;
	int failures = 0;

	while(roomCount < roomQuota){
		Room newRoom = generateNewRoom(map);
		if(isLegalRoom(&newRoom,map)==0){
			failures = 0;
			map -> room[roomCount++] = newRoom;
			placeNewRoom(map, &newRoom);
		}else{
			failures++;
		}
		if(failures > FAILED_ROOM_ATTEMPTS_LIMIT){
			// printf("Too many failed placements. Here is the map.\n");
			// printMap(*map);
			failures = 0;
			roomCount = 0;
			initializeMap(map);
		}
	} 
}
Room generateNewRoom(Map *map){
	static Room newRoom;
	newRoom.position.x = (rand() % MAPWIDTH);
	newRoom.position.y = (rand() % MAPHEIGHT);
	newRoom.height = (rand() % (MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT) + MIN_ROOM_HEIGHT);
	newRoom.width = (rand() % (MAX_ROOM_WIDTH - MIN_ROOM_WIDTH) + MIN_ROOM_WIDTH);

	return newRoom;	
}
/*Returns 	0:Legal
			-1:Encountered Bedrock
			-2:Encountered floor of another room
*/
int isLegalRoom(Room *room, Map *map){
	Room newRoom = *room;
	// printf("Attempting room: %d,%d,%d,%d\n",newRoom.position.x,newRoom.position.y,newRoom.width,newRoom.height);
	
	int i, j;
	//Check for bedrock inside the room
	for(i=newRoom.position.y; i < newRoom.position.y + newRoom.height; ++i){
		for(j=newRoom.position.x; j < newRoom.position.x + newRoom.width; ++j){
			Block curBlock;
			map_getBlock(map,j,i,&curBlock);

			if(curBlock.type == bedrock){
				return -1; //Bedrock encountered 
			}
		}
	}

	//Check for floors around the room
	for(i=0;i<newRoom.height +2; ++i){
		for(j=0;j<newRoom.width +2; ++j){
			Block curBlock;
			map_getBlock(map,
				newRoom.position.x+j-1,
				newRoom.position.y+i-1,
				&curBlock);
			if(curBlock.type == floor){
				return -2;
			}
		}
	}
	return 0;
}
void placeNewRoom(Map *map, Room *room){
	int xPos = room->position.x;
	int yPos = room->position.y;
	int width = room->width;
	int height = room->height;
	int i,j;
	// printf("Placing room: %d,%d,%d,%d\n",room.position.x,room.position.y,room.width,room.height);
	for(i = yPos; i < yPos + height; ++i){
		for(j = xPos; j < xPos + width; ++j){
			Block newBlock = block_create(floor,0);
			map_setBlock(map,j,i,&newBlock);
		}
	}
}

//-----------------------------CORRIDORS------------------------
void populateWithCorridors(Map *map){
	// printRoomList(map->room);
	int i=1;
	while(!isSentinel(map->room[i])){
		Room curRoom = map->room[i];
		int j;
		Room closestRoom = map->room[0];
		Room secondClosestRoom = map->room[0];
		//Calculate the closest and second closest rooms
		for(j=1; j<i; j++){
			Room otherRoom = map->room[j];
			if(dist(&curRoom,&otherRoom)
				<dist(&curRoom,&closestRoom)){
				secondClosestRoom = closestRoom;
				closestRoom = otherRoom;
			}else if(dist(&curRoom,&otherRoom)
					<dist(&curRoom,&secondClosestRoom)){
				secondClosestRoom = otherRoom;
			}
		}
		// printf("Calculating corridors from room %d\n",i);
		Corridor newCorridor= *generateNewCorridor(
			curRoom.position, closestRoom.position);
		placeNewCorridor(newCorridor,map);
		if(rand()%SECOND_CLOSEST_ROOM_CONNECTION_ODDS == 0){
			newCorridor= *generateNewCorridor(
			curRoom.position, secondClosestRoom.position);
			placeNewCorridor(newCorridor,map);
		}
		i++;
	}
}
int dist(Room *r1, Room *r2){
	return 	abs(r1->position.x - r2->position.x)
		+	abs(r2->position.y - r2->position.y);
}
Corridor * generateNewCorridor(Coordinate c1, Coordinate c2){
	static Corridor cor;
	cor.start.x = c1.x;
	cor.start.y = c1.y;
	cor.midpoint.x = c2.x;
	cor.midpoint.y = c1.y;
	cor.end.x = c2.x;
	cor.end.y = c2.y;

	// printf("From (%d,%d) through (%d,%d) to (%d,%d)\n",
	// 	cor.start.x,cor.start.y,
	// 	cor.midpoint.x,cor.midpoint.y,
	// 	cor.end.x,cor.end.y);

	return &cor;
}
void placeNewCorridor(Corridor cor, Map *map){
	// printf("Placing corridor\n");

	int xDistance = cor.start.x - cor.midpoint.x;
	int yDistance = cor.end.y - cor.midpoint.y;

	placePartialCorridor(cor.midpoint,xDistance,true,map);
	placePartialCorridor(cor.midpoint,yDistance,false,map);
}
//TODO Refactor this by using a "Place region" function
void placePartialCorridor(Coordinate origin, int dist, bool horizontal, Map *map){
	int incrementerVertical;
	int incrementerHorizontal;
	if(dist == 0){
		return;
	}
	if(horizontal){
		incrementerVertical = 0;
		incrementerHorizontal = dist/abs(dist);
	}else{
		incrementerVertical = dist/abs(dist);
		incrementerHorizontal = 0;
	}
	int i;
	for(i=0;i<=abs(dist);++i){
		int yCoord = origin.y + i*incrementerVertical;
		int xCoord = origin.x + i*incrementerHorizontal;
		Block curBlock;
		map_getBlock(map, xCoord, yCoord, &curBlock);
		if(curBlock.type == rock){
			Block replacementBlock = block_create(corridor,0);
			map_setBlock(map,xCoord,yCoord,&replacementBlock);
		}
	}
}
bool isSentinel(Room room){
	return room.height == -1;
}


//--------------------------STAIRS------------------------------

void populateWithStairs(Map *map){
	enum BlockType canReplace[] = {floor,corridor};
	assignTypeToRandomBlock(upstairs,canReplace,2,map);
	assignTypeToRandomBlock(downstairs,canReplace,2,map);
}
void assignTypeToRandomBlock(enum BlockType toPlace, enum BlockType canReplace[], int canReplaceSize, Map *map){
	while(true){
		int yCoord = rand()%MAPHEIGHT;
		int xCoord = rand()%MAPWIDTH;
		Block curBlock;
		map_getBlock(map,xCoord,yCoord,&curBlock);

		//check if it is permissible to replace the selected block
		int i;
		for(i=0;i<canReplaceSize; ++i){
			if(canReplace[i] == curBlock.type){
				Block newTypeBlock;
				map_getBlock(map,xCoord,yCoord,&newTypeBlock);
				newTypeBlock.type = toPlace;
				map_setBlock(map,xCoord,yCoord,&newTypeBlock);
				return;
			}
		}
		//The chosen block was of a type not in the canReplace list.
	}
}

//-----------------------------Entities-------------------------

void populateWithPC(Map * map){
	map->pcPos.x = map->room[0].position.x;
	map->pcPos.y = map->room[0].position.y;
}

//-----------------------------PRINTING-------------------------

//TODO Refactor this stuff into display
void printMap(Map * map){
	display_map(map);
}

void printRoomList(Room *roomList){
	display_room_list(roomList);
}


bool isOnBorder(Coordinate point, Coordinate ul, Coordinate lr){
	return	point.x==ul.x
		||	point.x==lr.x
		||  point.y==ul.y
		|| 	point.y==lr.y;
}

/*
string must point to a char array ending in a newline character
*/
void glog(char *string){
	fprintf(stderr,string);
}


//---------------------------Map Files-------------------------------------


int openFile(char * mode){
	int l = strlen(getenv("HOME")) + strlen(FILE_PATH) + 1; 
	char * filePath = malloc(l);
	strcpy(filePath,getenv("HOME"));
	strcat(filePath,FILE_PATH);
	if(!(fp=fopen(filePath,mode))){
		fprintf(stderr, "Failed to open file.\n");
		return -1;
	}
	free(filePath);
	return 0;
}
int closeFile(){
	if(fclose(fp)!=0){
		fprintf(stderr, "Failed to close file\n");
		return -1;
	}
	return 0;
}
int readFile(Map * newMap){
	// openFile("rb");
	// char data1, data2;
	// fread(&data1,1,1,fp);
	// fread(&data2,1,1,fp);
	// printf("Read from file: %c, %c.\n", data1, data2);
	char uselessInfoIn[16];
	uint32_t fileSizeIn;
	uint8_t pcPosXIn;
	uint8_t pcPosYIn;
	uint8_t hardnessesIn[MAPHEIGHT][MAPWIDTH];
	uint16_t roomCountIn;
	uint8_t * roomSpecsIn;//Remember to malloc AND free this sucker
	uint16_t upStairCountIn;
	uint8_t * upStairSpecsIn;//And this one
	uint16_t downStairCountIn;
	uint8_t * downStairSpecsIn;//And this one!


	openFile("rb");
	//Header things
	fread(&uselessInfoIn,1,16,fp);
	for(int i=0; i<16; ++i){
		// printf("%c\n", uselessInfoIn[i]);
	}
	// printf("-------\n");

	//File size
	fread(&fileSizeIn,4,1,fp);
	fileSizeIn = be32toh(fileSizeIn);
	// printf("Size:%u\n",fileSizeIn);

	//PC Coordinate
	fread(&pcPosXIn,1,1,fp);
	fread(&pcPosYIn,1,1,fp);
	// printf("PC is at: %d, %d\n",pcPosXIn,pcPosYIn);

	//Hardness
	int i,j;
	for(i=0; i<MAPHEIGHT; ++i){
		for(j=0; j<MAPWIDTH; ++j){
			fread(&hardnessesIn[i][j],1,1,fp);
			// printf("%3d", hardnessesIn[i][j]);
		}
		// printf("\n");
	}

	//Room count
	fread(&roomCountIn,2,1,fp);
	roomCountIn = be16toh(roomCountIn);
	// printf("Rooms:%u\n",roomCountIn);

	//Room specs
	roomSpecsIn = malloc(4*roomCountIn);
	// printf("Room specs:\n");
	for(i=0; i<4*roomCountIn; ++i){
		fread(&roomSpecsIn[i],1,1,fp);
		// printf("%d|%d\n",i,roomSpecsIn[i]);
	}

	//Up stairs count
	fread(&upStairCountIn,2,1,fp);
	upStairCountIn = be16toh(upStairCountIn);
	// printf("Up stairs:%d\n", upStairCountIn);

	//Up stairs specs
	upStairSpecsIn = malloc(upStairCountIn*2);
	// printf("Up stair specs:\n");
	for(i=0;i<upStairCountIn*2;++i){
		fread(&upStairSpecsIn[i],1,1,fp);
		// printf("%d|%d\n", i,upStairSpecsIn[i]);
	}


	//Down stairs count
	fread(&downStairCountIn,2,1,fp);
	downStairCountIn = be16toh(downStairCountIn);
	// printf("Down stairs:%d\n", downStairCountIn);

	//Down stairs specs
	downStairSpecsIn = malloc(downStairCountIn*2);
	// printf("Down stair specs:\n");
	for(i=0;i<downStairCountIn*2;++i){
		fread(&downStairSpecsIn[i],1,1,fp);
		// printf("%d|%d\n",i,downStairSpecsIn[i] );
	}


	//----Construct map from extracted values----

	initializeMap(newMap);
	//PC Position
	newMap->pcPos.x=pcPosXIn;
	newMap->pcPos.y=pcPosYIn;
	//Hardness matrix
	for(i=0;i<MAPHEIGHT;++i){
		for(j=0;j<MAPWIDTH;++j){
			enum BlockType newType;
			uint8_t hardness = hardnessesIn[i][j];
			if(hardness == 0){
				newType = corridor;
			}else if(hardness == 255){
				newType = bedrock;
			}else{
				newType = rock;
			}
			Block createBlock = block_create(newType,hardness);
			map_setBlock(newMap,j,i,&createBlock);
		}
	}
	//Rooms
	int k;
	for(i=0; i<roomCountIn; ++i){
		Room newRoom;
		newRoom.position.x = roomSpecsIn[4*i];
		newRoom.position.y = roomSpecsIn[4*i+1];
		newRoom.width = roomSpecsIn[4*i+2];
		newRoom.height = roomSpecsIn[4*i+3];
		newMap->room[i] = newRoom;

		for(j=0; j<newRoom.height; ++j){
			for(k=0; k<newRoom.width; ++k){
				placeNewRoom(newMap, &newRoom);
			}
		}
	}

	for(i=0; i<upStairCountIn; ++i){
		Block blockToStairs;
		map_getBlock(newMap,upStairSpecsIn[2*i],
			upStairSpecsIn[2*i+1],
			&blockToStairs);
		blockToStairs.type = upstairs;
		map_setBlock(newMap,upStairSpecsIn[2*i],
			upStairSpecsIn[2*i+1],
			&blockToStairs);
	}

	for(i=0; i<downStairCountIn; ++i){
		Block blockToStairs;
		map_getBlock(newMap,downStairSpecsIn[2*i],
			downStairSpecsIn[2*i+1],
			&blockToStairs);
		blockToStairs.type = downstairs;
		map_setBlock(newMap,downStairSpecsIn[2*i],
			downStairSpecsIn[2*i+1],
			&blockToStairs);
	}

	free(roomSpecsIn);
	free(upStairSpecsIn);
	free(downStairSpecsIn);

	// printMap(&newMap);

	closeFile();
	return 0;
}
int writeFile(Map * theMap){
	openFile("wb");

	//Things to write
	char uselessInfoOut[17] = "RLG327-S2019\0\0\0\0";
	uint32_t fileSizeOut;						//Calculate (Last)
	uint8_t pcPosXOut = theMap->pcPos.x;
	uint8_t pcPosYOut = theMap->pcPos.y;
	uint8_t hardnessesOut[MAPHEIGHT][MAPWIDTH];	//Fill (Map passover 1)
	uint16_t roomCountOut = 0;	//Count (Room passover)
	// uint8_t * roomSpecsOut;		//Fill  (Room passover)
	uint16_t upStairCountOut=0;	//Count (Map passover 1)
	// uint8_t * upStairSpecsOut;	//Fill  (Map passover 2)
	uint16_t downStairCountOut=0;//Count (Map passover 1)
	// uint8_t * downStairSpecsOut;//Fill  (Map passover 2)

	//Map passover 1
	int i,j;
	for(i=0;i<MAPHEIGHT;++i){
		for(j=0;j<MAPWIDTH;++j){
			Block curBlock;
			map_getBlock(theMap,j,i,&curBlock);
			hardnessesOut[i][j]=curBlock.hardness;
			if(curBlock.type == upstairs){
				upStairCountOut++;
			}else if(curBlock.type == downstairs){
				downStairCountOut++;
			}
		}
	}
	uint8_t * upStairSpecsOut;
	uint8_t  * downStairSpecsOut;
	if(!(upStairSpecsOut = malloc(2*upStairCountOut))
	|| !(downStairSpecsOut = malloc(2*downStairCountOut))){
		printf("Failed to malloc.\n");
		return -1;
	}


	//Map passover 2
	int upIndex = 0;
	int downIndex = 0;
	for(i=0;i<MAPHEIGHT;++i){
		for(j=0;j<MAPWIDTH;++j){
			Block curBlock;
			map_getBlock(theMap,j,i,&curBlock);
			if(curBlock.type == upstairs){
				upStairSpecsOut[upIndex++] = j;
				upStairSpecsOut[upIndex++] = i;
			}else if(curBlock.type == downstairs){
				downStairSpecsOut[downIndex++] = j;
				downStairSpecsOut[downIndex++] = i;
			}
		}
	}

	//Room passover 1
	for(i=0; i<MAX_ROOM_COUNT; ++i){
		if(isSentinel(theMap->room[i])){
			break;
		}
		roomCountOut++;
	}
	uint8_t * roomSpecsOut = malloc(4*roomCountOut);

	//Room passover 2
	for(i=0;i<roomCountOut;++i){
		roomSpecsOut[4*i]=theMap->room[i].position.x;
		roomSpecsOut[4*i+1]=theMap->room[i].position.y;
		roomSpecsOut[4*i+2]=theMap->room[i].width;
		roomSpecsOut[4*i+3]=theMap->room[i].height;
	}

	fileSizeOut = 1708
	+ 4*roomCountOut
	+ 2*upStairCountOut
	+ 2*downStairCountOut;




	//--------Write calculated values to file-----------

	// printf("Writing header stuff: %s\n", uselessInfoOut);
	for(i=0;i<16; ++i){
		fwrite(uselessInfoOut+i,1,1,fp);
	}

	// printf("Writing file size: %d\n", fileSizeOut);
	uint32_t tmp32 = htobe32(fileSizeOut);
	fwrite(&tmp32,1,4,fp);

	// printf("Writing PC position: (%d,%d)\n", pcPosXOut, pcPosYOut);
	fwrite(&pcPosXOut,1,1,fp);
	fwrite(&pcPosYOut,1,1,fp);

	// printf("Writing hardness array:\n");
	for(i=0; i<MAPHEIGHT; ++i){
		for(j=0; j<MAPWIDTH; ++j){
			// printf("%3d",hardnessesOut[i][j]);
			fwrite(&hardnessesOut[i][j],1,1,fp);
		}
		// printf("\n");
	}

	// printf("Writing room count: %d\n", roomCountOut);
	uint16_t tmp16 = htobe16(roomCountOut);
	fwrite(&tmp16,1,2,fp);

	// printf("Writing room specs:");
	// for(i=0;i<roomCountOut;++i){
	// 	printf("X|%d Y|%d W|%d H|%d\n",
	// 		roomSpecsOut[4*i],
	// 		roomSpecsOut[4*i+1],
	// 		roomSpecsOut[4*i+2],
	// 		roomSpecsOut[4*i+3]);
	// }
	for(i=0;i<4*roomCountOut;++i){
		fwrite(&roomSpecsOut[i],1,1,fp);
	}

	// printf("Writing stairs:\n");
	// printf("Stair count: Up|%d Down|%d\n", upStairCountOut,downStairCountOut);
	tmp16 = htobe16(upStairCountOut);
	fwrite(&tmp16,1,2,fp);
	for(i=0;i<upStairCountOut;++i){
		// printf("Up: X|%d Y|%d\n",
		// 	upStairSpecsOut[2*i],upStairSpecsOut[2*i+1]);
		fwrite(&upStairSpecsOut[2*i],1,1,fp);
		fwrite(&upStairSpecsOut[2*i+1],1,1,fp);
	}

	tmp16 = htobe16(downStairCountOut);
	fwrite(&tmp16,1,2,fp);
	for(i=0;i<downStairCountOut;++i){
		// printf("Down: X|%d Y|%d\n",
		// 	downStairSpecsOut[2*i],downStairSpecsOut[2*i+1]);
		fwrite(&downStairSpecsOut[2*i],1,1,fp);
		fwrite(&downStairSpecsOut[2*i+1],1,1,fp);
	}

	free(upStairSpecsOut);
	free(downStairSpecsOut);
	free(roomSpecsOut);
	closeFile();

	return 0;
}
