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
#define MAX_ROOM_HEIGHT 14	//Exclusive

#define MIN_ROOM_COUNT 1
#define MAX_ROOM_COUNT 1

//TODO Introduce room border
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



int attemptNewRoom(struct Map *map);
void placeNewRoom(struct Map *map, struct Room room);
void printMap(struct Map map);
char getVisual(enum BlockType type);
void testMapPrint(void);



int main(int argc, char *argv[]){
	struct Map theMap;
	int i,j;
	printf("Initializing map...\n");
	for(i = 0; i < MAPWIDTH; ++i){
		for(j = 0; j < MAPHEIGHT; ++j){
			struct Block newBlock;
			if((i==0||i==MAPWIDTH-1)||(j==0||j==MAPHEIGHT-1)){
				newBlock.type = bedrock;
			}else{
				newBlock.type = rock;
			}
			newBlock.isRoomBorder = false;
			theMap.block[i][j] = newBlock;
			printf("X|%d  Y|%d  T|%c \n",i,j,getVisual(newBlock.type));

		}
	}

	printf("Map initialized\n");
	srand(time(0));
		int status = -1;
		int roomCount = 0;
	for(i = 0; i < 5; ++i){
		status = attemptNewRoom(&theMap);
		printf("%d\n", status);
		if(status == 0){
			++roomCount;
		}
		printMap(theMap);
		printf("%d rooms\n", roomCount);
	}
	
	return 0;
}



int attemptNewRoom(struct Map *map){
	struct Room newRoom;
	newRoom.position.x = (rand() % MAPWIDTH);
	newRoom.position.y = (rand() % MAPHEIGHT);
	newRoom.height = (rand() % (MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT) + MIN_ROOM_HEIGHT);
	newRoom.width = (rand() % (MAX_ROOM_WIDTH - MIN_ROOM_WIDTH) + MIN_ROOM_WIDTH);

	printf("Attempting room: %d,%d,%d,%d\n",newRoom.position.x,newRoom.position.y,newRoom.width,newRoom.height);

	//Check if room touches immutible blocks. This also covers the case where the room
	//extends out of bounds.
	int i, j;
	for(i=newRoom.position.x; i < newRoom.position.x + newRoom.width; ++i){
		for(j=newRoom.position.y; j < newRoom.position.y + newRoom.height; ++j){
			struct Block curBlock = map->block[i][j];
			if(curBlock.type == bedrock){
				return -1; //Bedrock encountered
			}else if(curBlock.isRoomBorder){
//TODO Room borders can overlap. Add isBorderOfRegion function
				return -2; //Room border encountered
			}else if(curBlock.type == floor){
				return -3; //Another room's floor was encountered
			}
		}
	}

	placeNewRoom(map,newRoom);
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