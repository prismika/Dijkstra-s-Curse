#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAPWIDTH 80
#define MAPHEIGHT 21
//Keep in mind these include the room border
#define MIN_ROOM_WIDTH 6 //6
#define MIN_ROOM_HEIGHT 5 //5
#define MAX_ROOM_WIDTH 18 //Exclusive
#define MAX_ROOM_HEIGHT 12	//Exclusive

#define MIN_ROOM_COUNT 6
#define MAX_ROOM_COUNT 9 //Exclusive
#define FAILED_ROOM_ATTEMPTS_LIMIT 32

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
};

//TODO Flip x and y everywhere
struct Map{
	struct Block block[MAPWIDTH][MAPHEIGHT];
};

struct Coordinate{
	int x;
	int y;
};

struct Room{
	struct Coordinate position;
	int height;
	int width;
};

struct Corridor{
	struct Coordinate start;
	struct Coordinate midpoint;
	struct Coordinate end;
};


void initializeMap(struct Map *map);
void populateWithRooms(struct Map *map);
int attemptNewRoom(struct Map *map);
int isLegalRoom(struct Room *room, struct Map *map);
void placeNewRoom(struct Map *map, struct Room room);
void printMap(struct Map map);
char getVisual(enum BlockType type);
void testMapPrint(void);
bool isOnBorder(struct Coordinate point, struct Coordinate ul, struct Coordinate lr);


int main(int argc, char *argv[]){
	srand(time(0));

	struct Map theMap;
	initializeMap(&theMap);
	populateWithRooms(&theMap);
	printMap(theMap);
	
	return 0;
}

void initializeMap(struct Map *map){
	printf("Initializing map...\n");

	int i,j;
	for(i = 0; i < MAPWIDTH; ++i){
		for(j = 0; j < MAPHEIGHT; ++j){
			struct Block newBlock;
			if((i==0||i==MAPWIDTH-1)||(j==0||j==MAPHEIGHT-1)){
				newBlock.type = bedrock;
			}else{
				newBlock.type = rock;
			}
			newBlock.isRoomBorder = false;
			map->block[i][j] = newBlock;
		}
	}

	printf("Map initialized\n");
}

void populateWithRooms(struct Map *map){
	int roomQuota = (rand()%(MAX_ROOM_COUNT - MIN_ROOM_COUNT)) + MIN_ROOM_COUNT;
	int roomCount = 0;
	int failures = 0;
	while(roomCount < roomQuota){
		int attemptResult = attemptNewRoom(map);
		if(attemptResult == 0){
			failures = 0;
			roomCount++;
		}else{
			failures++;
		}
		if(failures > FAILED_ROOM_ATTEMPTS_LIMIT){
			printf("Too many failed placements. Here is the map.\n");
			printMap(*map);
			failures = 0;
			roomCount = 0;
			initializeMap(map);
		}
	}
}

int attemptNewRoom(struct Map *map){
	struct Room newRoom;
	newRoom.position.x = (rand() % MAPWIDTH);
	newRoom.position.y = (rand() % MAPHEIGHT);
	newRoom.height = (rand() % (MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT) + MIN_ROOM_HEIGHT);
	newRoom.width = (rand() % (MAX_ROOM_WIDTH - MIN_ROOM_WIDTH) + MIN_ROOM_WIDTH);

	printf("Attempting room: %d,%d,%d,%d\n",newRoom.position.x,newRoom.position.y,newRoom.width,newRoom.height);

	int legality = isLegalRoom(&newRoom,map);
	if(legality==0) placeNewRoom(map,newRoom);
	return legality;
}

/*Returns 	0:Legal
			-1:Encountered Bedrock
			-2:Encountered border of another room
			-3:Encountered floor of another room
*/
int isLegalRoom(struct Room *room, struct Map *map){
	int i, j;
	struct Room newRoom = *room;
	for(i=newRoom.position.x; i < newRoom.position.x + newRoom.width; ++i){
		for(j=newRoom.position.y; j < newRoom.position.y + newRoom.height; ++j){
			struct Block curBlock = map->block[i][j];
			struct Coordinate curBlockCoord = {i,j};
			struct Coordinate lowerRight = {newRoom.position.x + newRoom.width-1,newRoom.position.y + newRoom.height-1};

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
	printf("Placing room: %d,%d,%d,%d\n",room.position.x,room.position.y,room.width,room.height);
	for(i=room.position.x; i < room.position.x + room.width; ++i){
		for(j=room.position.y; j < room.position.y + room.height; ++j){
			struct Block newBlock;
			if(	i==room.position.x || i==room.position.x+room.width-1
			||	j==room.position.y || j==room.position.y+room.height-1){
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

void printMap(struct Map map){
	int i,j;
	for(j=0;j<MAPHEIGHT;++j){
		for(i=0; i<MAPWIDTH; ++i){
			struct Block curBlock = map.block[i][j];
			char blockVisual[1];
			blockVisual[0] = getVisual(curBlock.type);

			printf(blockVisual);
		}
		printf("\n");
	}
}

char getVisual(enum BlockType type){
	switch(type){
		case rock:
			return ' ';
			break;
		case floor:
			return '.';
			break;
		default:
			return '!';
	}
	return '!';
}

//TODO Overload this and refactor above
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