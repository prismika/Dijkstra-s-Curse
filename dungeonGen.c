#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <endian.h>
#include "mapElements.h"

#define MAPWIDTH 80
#define MAPHEIGHT 21

#define MIN_ROOM_WIDTH 4 //4
#define MIN_ROOM_HEIGHT 3 //3
#define MAX_ROOM_WIDTH 12 //Exclusive
#define MAX_ROOM_HEIGHT 8	//Exclusive

#define MIN_ROOM_COUNT 6 //6
#define MAX_ROOM_COUNT 10 //Exclusive
#define FAILED_ROOM_ATTEMPTS_LIMIT 32

#define SECOND_CLOSEST_ROOM_CONNECTION_ODDS 3//One in...

#define FILE_PATH "/.rlg327/dungeon"
//"/.rlg327/jerBear/04.rlg327"
FILE * fp;

struct Map{
	struct Block block[MAPHEIGHT][MAPWIDTH];
	struct Room room[MAX_ROOM_COUNT+1];//Extra spot for sentinel value
	struct Coordinate pcPos;
};
enum IncomingCommand{
	nothing,
	save,
	load,
	load_save
};

int parseCLI(int argc, char * argv[], enum IncomingCommand * command, long * seed);

void generateNewMap(struct Map * map, long seed);
void initializeMap(struct Map *map);
void populateWithRooms(struct Map *map);
struct Room generateNewRoom(struct Map *map);
int isLegalRoom(struct Room *room, struct Map *map);
void placeNewRoom(struct Map *map, struct Room *room);

void populateWithCorridors(struct Map *map);
int dist(struct Room *r1, struct Room *r2);
struct Corridor * generateNewCorridor(struct Coordinate c1, struct Coordinate c2);
void placeNewCorridor(struct Corridor cor, struct Map *map);
void placePartialCorridor(struct Coordinate origin, int dist, bool horizontal, struct Map *map);
bool isSentinel(struct Room room);

void populateWithStairs(struct Map *map);
void assignTypeToRandomBlock(enum BlockType toPlace, enum BlockType canReplace[], int canReplaceSize, struct Map *map);

void printMap(struct Map * map);
void printRoomList(struct Room *roomList);
char getVisual(enum BlockType type);
void glog(char *string);

void testMapPrint(void);
bool isOnBorder(struct Coordinate point, struct Coordinate ul, struct Coordinate lr);
//-----------------------Constructors----------------------------
struct Block newBlock(enum BlockType blockType, uint8_t hardness);

int openFile(char * mode);
int readFile(struct Map * newMap);
int closeFile(void);
int writeFile(struct Map * theMap);

//TODO Debug mode
int main(int argc, char *argv[]){

	long seed;
	enum IncomingCommand whatsup;
	if(parseCLI(argc,argv,&whatsup,&seed)!=0){
		printf("Incorrect usage\n");
		return -1;
	}
	struct Map theMap;
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
	}
	
	

	
	writeFile(&theMap);
	readFile(&theMap);
	printMap(&theMap);

	return 0;
}

//TODO Custom seeds again
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
	}else{
		return -2;
	}
	return 0;
}


void generateNewMap(struct Map * map, long seed){
	srand(seed);
	initializeMap(map);
	populateWithRooms(map);
	populateWithCorridors(map);
	populateWithStairs(map);
}
void initializeMap(struct Map *map){
	// printf("Initializing map...\n");

	int i,j;
	for(i = 0; i < MAPHEIGHT; ++i){
		for(j = 0; j < MAPWIDTH; ++j){
			struct Block block;
			if((i==0||i==MAPHEIGHT-1)||(j==0||j==MAPWIDTH-1)){
				block = newBlock(bedrock,255);
			}else{
				block = newBlock(rock,100);
			}
			map->block[i][j] = block;
		}
	}

	for(i=0;i<MAX_ROOM_COUNT+1;i++){
		struct Room sentinelRoom;
		sentinelRoom.height=-1;
		sentinelRoom.width=-1;
		sentinelRoom.position.x = 0;
		sentinelRoom.position.y = 0;
		map->room[i] = sentinelRoom;
	}

	map -> pcPos.x = -1;
	map -> pcPos.y = -1;

	// printf("Map initialized\n");
}

//----------------------------ROOMS------------------------------
void populateWithRooms(struct Map *map){
	int roomQuota = (rand()%(MAX_ROOM_COUNT - MIN_ROOM_COUNT)) + MIN_ROOM_COUNT;
	int roomCount = 0;
	int failures = 0;

	while(roomCount < roomQuota){
		struct Room newRoom = generateNewRoom(map);
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
struct Room generateNewRoom(struct Map *map){
	static struct Room newRoom;
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
int isLegalRoom(struct Room *room, struct Map *map){
	struct Room newRoom = *room;
	// printf("Attempting room: %d,%d,%d,%d\n",newRoom.position.x,newRoom.position.y,newRoom.width,newRoom.height);
	
	int i, j;
	//Check for bedrock inside the room
	for(i=newRoom.position.y; i < newRoom.position.y + newRoom.height; ++i){
		for(j=newRoom.position.x; j < newRoom.position.x + newRoom.width; ++j){
			struct Block curBlock = map->block[i][j];

			if(curBlock.type == bedrock){
				return -1; //Bedrock encountered 
			}
		}
	}

	//Check for floors around the room
	for(i=0;i<newRoom.height +2; ++i){
		for(j=0;j<newRoom.width +2; ++j){
			struct Block curBlock
				= map->block
					[newRoom.position.y+i-1]
					[newRoom.position.x+j-1];
			if(curBlock.type == floor){
				return -2;
			}
		}
	}
	return 0;
}
void placeNewRoom(struct Map *map, struct Room *room){
	int xPos = room->position.x;
	int yPos = room->position.y;
	int width = room->width;
	int height = room->height;
	int i,j;
	// printf("Placing room: %d,%d,%d,%d\n",room.position.x,room.position.y,room.width,room.height);
	for(i = yPos; i < yPos + height; ++i){
		for(j = xPos; j < xPos + width; ++j){
			struct Block block = newBlock(floor,0);
			map->block[i][j] = block;
		}
	}
}

//-----------------------------CORRIDORS------------------------
void populateWithCorridors(struct Map *map){
	// printRoomList(map->room);
	int i=1;
	while(!isSentinel(map->room[i])){
		struct Room curRoom = map->room[i];
		int j;
		struct Room closestRoom = map->room[0];
		struct Room secondClosestRoom = map->room[0];
		//Calculate the closest and second closest rooms
		for(j=1; j<i; j++){
			struct Room otherRoom = map->room[j];
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
		struct Corridor newCorridor= *generateNewCorridor(
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
int dist(struct Room *r1, struct Room *r2){
	return 	abs(r1->position.x - r2->position.x)
		+	abs(r2->position.y - r2->position.y);
}
struct Corridor * generateNewCorridor(struct Coordinate c1, struct Coordinate c2){
	static struct Corridor cor;
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
void placeNewCorridor(struct Corridor cor, struct Map *map){
	// printf("Placing corridor\n");

	int xDistance = cor.start.x - cor.midpoint.x;
	int yDistance = cor.end.y - cor.midpoint.y;

	placePartialCorridor(cor.midpoint,xDistance,true,map);
	placePartialCorridor(cor.midpoint,yDistance,false,map);
}
//TODO Refactor this by using a "Place region" function
void placePartialCorridor(struct Coordinate origin, int dist, bool horizontal, struct Map *map){
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
		if(map -> block[yCoord][xCoord].type == rock){
			struct Block replacementBlock = newBlock(corridor,0);
			map -> block[yCoord][xCoord] = replacementBlock;
		}
	}
}
bool isSentinel(struct Room room){
	return room.height == -1;
}


//--------------------------STAIRS------------------------------

void populateWithStairs(struct Map *map){
	enum BlockType canReplace[] = {floor,corridor};
	assignTypeToRandomBlock(upstairs,canReplace,2,map);
	assignTypeToRandomBlock(downstairs,canReplace,2,map);
}
void assignTypeToRandomBlock(enum BlockType toPlace, enum BlockType canReplace[], int canReplaceSize, struct Map *map){
	while(true){
		int yCoord = rand()%MAPHEIGHT;
		int xCoord = rand()%MAPWIDTH;
		enum BlockType type = map -> block[yCoord][xCoord].type;

		//check if it is permissible to replace the selected block
		int i;
		for(i=0;i<canReplaceSize; ++i){
			if(canReplace[i] == type){
				map -> block[yCoord][xCoord].type = toPlace;
				return;
			}
		}
		//The chosen block was of a type not in the canReplace list.
	}
}

//-----------------------------PRINTING-------------------------

void printMap(struct Map * map){
	int i,j;
	for(i=0;i<MAPHEIGHT;++i){
		for(j=0; j<MAPWIDTH; ++j){
			struct Block curBlock = map->block[i][j];
			char blockVisual[1];
			blockVisual[0] = getVisual(curBlock.type);

			printf(blockVisual);
		}
		printf("\n");
	}
}
void printRoomList(struct Room *roomList){
	printf("Rooms:\n");

	int i;
	for(i=0;i<MAX_ROOM_COUNT;++i){
		struct Room curRoom = roomList[i];
		printf("\tRoom %d: X|%d Y|%d W|%d H|%d\n",
			i,curRoom.position.x,curRoom.position.y,
			curRoom.width,curRoom.height);
	}
}
char getVisual(enum BlockType type){
	switch(type){
		case rock:		return ' ';
		case floor:		return '.';
		case corridor:	return '#';
		case upstairs:	return '<';
		case downstairs:return '>';
		case bedrock: 	return '|';
		default:		return '!';
	}
}
bool isOnBorder(struct Coordinate point, struct Coordinate ul, struct Coordinate lr){
	return	point.x==ul.x
		||	point.x==lr.x
		||  point.y==ul.y
		|| 	point.y==lr.y;
}
void testMapPrint(void){
	struct Map map;
	int i,j;
	for(i=0;i<MAPHEIGHT;++i){
		for(j=0; j<MAPWIDTH; ++j){
			map.block[i][j].type = floor;
		}
	}
	printMap(&map);
}

/*
string must point to a char array ending in a newline character
*/
void glog(char *string){
	fprintf(stderr,string);
}


//------------------------------Constructors---------------------------------------


struct Block newBlock(enum BlockType blockType, uint8_t hardness){
	struct Block retBlock;
	retBlock.type = blockType;
	retBlock.hardness = hardness;
	return retBlock;
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
int readFile(struct Map * newMap){
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
			enum BlockType newBlockType;
			uint8_t hardness = hardnessesIn[i][j];
			if(hardness == 0){
				newBlockType = corridor;
			}else if(hardness == 255){
				newBlockType = bedrock;
			}else{
				newBlockType = rock;
			}
			struct Block createBlock = newBlock(newBlockType,hardness);
			newMap->block[i][j] = createBlock;
		}
	}
	//Rooms
	int k;
	for(i=0; i<roomCountIn; ++i){
		struct Room newRoom;
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
	// printRoomList(newMap->room);

	for(i=0; i<upStairCountIn; ++i){
		newMap->block
			[upStairSpecsIn[2*i+1]]
			[upStairSpecsIn[2*i]]
			.type = upstairs;
	}

	for(i=0; i<downStairCountIn; ++i){
		newMap->block
			[downStairSpecsIn[2*i+1]]
			[downStairSpecsIn[2*i]]
			.type = downstairs;
	}

	free(roomSpecsIn);
	free(upStairSpecsIn);
	free(downStairSpecsIn);

	// printMap(&newMap);

	closeFile();
	return 0;
}
int writeFile(struct Map * theMap){
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
			struct Block curBlock = theMap->block[i][j];
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
			struct Block curBlock = theMap->block[i][j];
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
