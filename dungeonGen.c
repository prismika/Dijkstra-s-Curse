#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>

#define MAPWIDTH 80
#define MAPHEIGHT 21
//Keep in mind these measurements include the room border
#define MIN_ROOM_WIDTH 6 //6
#define MIN_ROOM_HEIGHT 5 //5
#define MAX_ROOM_WIDTH 18 //Exclusive
#define MAX_ROOM_HEIGHT 12	//Exclusive

#define MIN_ROOM_COUNT 6 //6
#define MAX_ROOM_COUNT 10 //Exclusive
#define FAILED_ROOM_ATTEMPTS_LIMIT 32

#define SECOND_CLOSEST_ROOM_CONNECTION_ODDS 3

enum BlockType{
	rock,
	bedrock,
	corridor,
	floor,
	upstairs,
	downstairs
};

struct Block{
	enum BlockType type;
	bool isRoomBorder;
	uint8_t hardness;
};

//TODO Flip x and y everywhere
struct Map{
	struct Block block[MAPHEIGHT][MAPWIDTH];
};

struct Coordinate{
	int y;
	int x;
};

struct Room{
	struct Coordinate position;
	int height;
	int width;
};

/*It is assumed that the corridor goes horizontally from start
and vertically to end*/
struct Corridor{
	struct Coordinate start;
	struct Coordinate midpoint;
	struct Coordinate end;
};


void initializeMap(struct Map *map);
struct Room * populateWithRooms(struct Map *map);
struct Room * generateNewRoom(struct Map *map);
int isLegalRoom(struct Room *room, struct Map *map);
void placeNewRoom(struct Map *map, struct Room room);

void populateWithCorridors(struct Map *map, struct Room room[]);
int dist(struct Room *r1, struct Room *r2);
struct Corridor * generateNewCorridor(struct Coordinate c1, struct Coordinate c2);
void placeNewCorridor(struct Corridor cor, struct Map *map);
void placePartialCorridor(struct Coordinate origin, int dist, bool horizontal, struct Map *map);
bool isSentinel(struct Room room);
void erodeCorridors(struct Map *map);

void populateWithStairs(struct Map *map);
void assignTypeToRandomBlock(enum BlockType toPlace, enum BlockType canReplace[], int canReplaceSize, struct Map *map);

void printMap(struct Map map);
void printRoomList(struct Room *roomList);
char getVisual(enum BlockType type);

void testMapPrint(void);
bool isOnBorder(struct Coordinate point, struct Coordinate ul, struct Coordinate lr);
//-----------------------Constructors----------------------------
struct Block newBlock(enum BlockType blockType, uint8_t hardness, bool isRoomBorder);

//TODO Debug mode
int main(int argc, char *argv[]){
	long seed;
	if(argc == 2){
		seed = atoi(argv[1]);
	}else{
		seed = time(0);
	}
	printf("Seed:%ld\n", seed);
	srand(seed);

	struct Map theMap;
	initializeMap(&theMap);
	struct Room *roomList = populateWithRooms(&theMap);
	populateWithCorridors(&theMap,roomList);
	// TODO erodeCorridors(&theMap);
	populateWithStairs(&theMap);
	printMap(theMap);

	return 0;
}

void initializeMap(struct Map *map){
	// printf("Initializing map...\n");

	int i,j;
	for(i = 0; i < MAPHEIGHT; ++i){
		for(j = 0; j < MAPWIDTH; ++j){
			struct Block newBlock;
			if((i==0||i==MAPHEIGHT-1)||(j==0||j==MAPWIDTH-1)){
				newBlock.type = bedrock;
			}else{
				newBlock.type = rock;
			}
			newBlock.isRoomBorder = false;
			map->block[i][j] = newBlock;
		}
	}

	// printf("Map initialized\n");
}

//----------------------------ROOMS------------------------------

struct Room * populateWithRooms(struct Map *map){
	int roomQuota = (rand()%(MAX_ROOM_COUNT - MIN_ROOM_COUNT)) + MIN_ROOM_COUNT;
	int roomCount = 0;
	int failures = 0;
	static struct Room roomList[MAX_ROOM_COUNT+1];

	while(roomCount < roomQuota){
		struct Room *newRoom = generateNewRoom(map);
		if(isLegalRoom(newRoom,map)==0){
			failures = 0;
			roomList[roomCount++] = *newRoom;
			placeNewRoom(map,*newRoom);
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
	struct Room sentinelRoom;
	sentinelRoom.height = -1;
	roomList[roomQuota] = sentinelRoom;
	return roomList;//TODO For testing purposes
}

struct Room * generateNewRoom(struct Map *map){
	static struct Room newRoom;
	newRoom.position.x = (rand() % MAPWIDTH);
	newRoom.position.y = (rand() % MAPHEIGHT);
	newRoom.height = (rand() % (MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT) + MIN_ROOM_HEIGHT);
	newRoom.width = (rand() % (MAX_ROOM_WIDTH - MIN_ROOM_WIDTH) + MIN_ROOM_WIDTH);

	return &newRoom;	
}

/*Returns 	0:Legal
			-1:Encountered Bedrock
			-2:Encountered border of another room
			-3:Encountered floor of another room
*/
int isLegalRoom(struct Room *room, struct Map *map){
	struct Room newRoom = *room;
	// printf("Attempting room: %d,%d,%d,%d\n",newRoom.position.x,newRoom.position.y,newRoom.width,newRoom.height);
	
	int i, j;
	for(i=newRoom.position.y; i < newRoom.position.y + newRoom.height; ++i){
		for(j=newRoom.position.x; j < newRoom.position.x + newRoom.width; ++j){
			struct Block curBlock = map->block[i][j];
			struct Coordinate curBlockCoord;
			curBlockCoord.y=i;
			curBlockCoord.x=j;
			struct Coordinate lowerRight;
			lowerRight.y = newRoom.position.y + newRoom.height-1;
			lowerRight.x = newRoom.position.x + newRoom.width-1;


			if(curBlock.type == bedrock){
				return -1; //Bedrock encountered
			}else if(curBlock.isRoomBorder
					&&	!isOnBorder(curBlockCoord,newRoom.position,lowerRight)){
				return -2; //Room border encountered
			}else if(curBlock.type == floor){
				return -3; //Another room's floor was encountered
			}
		}
	}
	return 0;
}

void placeNewRoom(struct Map *map, struct Room room){
	int i,j;
	// printf("Placing room: %d,%d,%d,%d\n",room.position.x,room.position.y,room.width,room.height);
	for(i=room.position.y; i < room.position.y + room.height; ++i){
		for(j=room.position.x; j < room.position.x + room.width; ++j){
			struct Block newBlock;
			if(	i==room.position.y || i==room.position.y+room.height-1
			||	j==room.position.x || j==room.position.x+room.width-1){
				newBlock.type = rock;
				newBlock.isRoomBorder = true;
			}else{
				newBlock.type = floor;
				newBlock.isRoomBorder = false;
			}
			map->block[i][j] = newBlock;
		}
	}
}

//-----------------------------CORRIDORS------------------------

void populateWithCorridors(struct Map *map, struct Room room[]){
	// printRoomList(room);
	int i=1;
	while(!isSentinel(room[i])){
		struct Room curRoom = room[i];
		int j;
		struct Room closestRoom = room[0];
		struct Room secondClosestRoom = room[0];
		//Calculate the closest and second closest rooms
		for(j=1; j<i; j++){
			struct Room otherRoom = room[j];
			if(dist(&curRoom,&otherRoom)
				<dist(&curRoom,&closestRoom)){
				secondClosestRoom = closestRoom;
				closestRoom = otherRoom;
			}else if(dist(&curRoom,&otherRoom)
					<dist(&curRoom,&secondClosestRoom)){
				secondClosestRoom = otherRoom;
			}
		}
		printf("Calculating corridors from room %d\n",i);
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
	//TODO This is what technical debt looks like.
	//Put the offset where it belongs
	cor.start.x = c1.x+1;
	cor.start.y = c1.y+1;
	cor.midpoint.x = c2.x+1;
	cor.midpoint.y = c1.y+1;
	cor.end.x = c2.x+1;
	cor.end.y = c2.y+1;

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
			map -> block[yCoord][xCoord].type = corridor;
		}
	}
}

bool isSentinel(struct Room room){
	return room.height == -1;
}

int localeCount = 4;
enum BlockType locales[][3][3]={
	{{	rock,	rock,	rock},
	{	rock,	corridor,corridor},
	{	rock,	corridor,	rock}},

	{{	rock,	rock,	rock},
	{corridor,	corridor,rock},
	{	rock,	corridor,rock}},

	{{	rock,	corridor,rock},
	{corridor,	corridor,rock},
	{	rock,	rock,	rock}},

	{{	rock,	corridor,rock},
	{	rock,	corridor,corridor},
	{	rock,	rock,	rock}}
};
enum BlockType localeReplacements[][3][3]={
	{{	rock,	rock,	rock},
	{	rock,	rock,	corridor},
	{	rock,	corridor,corridor}},

	{{	rock,	rock,	rock},
	{corridor,	rock,	rock},
	{corridor,	corridor,rock}},

	{{rock,	corridor,rock},
	{corridor,corridor,rock},
	{	rock,	rock,	rock}},

	{{	rock,	corridor,rock},
	{	rock,	corridor,corridor},
	{	rock,	rock,	rock}}
};

void erodeCorridors(struct Map *map){

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

void printMap(struct Map map){
	int i,j;
	for(i=0;i<MAPHEIGHT;++i){
		for(j=0; j<MAPWIDTH; ++j){
			struct Block curBlock = map.block[i][j];
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
		case bedrock: 	return ' ';
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
	printMap(map);
}


//------------------------------Constructors---------------------------------------


struct Block newBlock(enum BlockType blockType, uint8_t hardness, bool isRoomBorder){
	struct Block retBlock;
	retBlock.type = blockType;
	retBlock.hardness = hardness;
	retBlock.isRoomBorder = false;
	return retBlock;
}