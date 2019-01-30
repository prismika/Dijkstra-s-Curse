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

#define MIN_ROOM_COUNT 6 //6
#define MAX_ROOM_COUNT 7 //Exclusive
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

/*It is assumed that the corridor goes horizontally to start
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
struct Corridor * generateNewCorridor(struct Coordinate c1, struct Coordinate c2);
void placeNewCorridor(struct Corridor cor, struct Map *map);
bool isSentinel(struct Room room);

void printMap(struct Map map);
void printRoomList(struct Room *roomList);
char getVisual(enum BlockType type);
void testMapPrint(void);
bool isOnBorder(struct Coordinate point, struct Coordinate ul, struct Coordinate lr);


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

struct Room * populateWithRooms(struct Map *map){
	int roomQuota = (rand()%(MAX_ROOM_COUNT - MIN_ROOM_COUNT)) + MIN_ROOM_COUNT;
	int roomCount = 0;
	int failures = 0;
	static struct Room roomList[MAX_ROOM_COUNT+1];//+1?

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
			printf("Too many failed placements. Here is the map.\n");
			printMap(*map);
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
	printf("Attempting room: %d,%d,%d,%d\n",newRoom.position.x,newRoom.position.y,newRoom.width,newRoom.height);
	
	int i, j;
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




void populateWithCorridors(struct Map *map, struct Room room[]){
	// struct Corridor cor;
	// cor.midpoint.x=1;
	// cor.midpoint.y=1;
	// cor.start.x=5;
	// cor.start.y=1;
	// cor.end.x=5;
	// cor.end.y=7;
	// placeNewCorridor(cor,map);
	printRoomList(room);
	int i=0;
	while(!isSentinel(room[i+1])){
		printf("Calculating corridor from room %d\n",i);
		struct Corridor newCorridor= *generateNewCorridor(
			room[i].position,room[i+1].position);
		if(i<1)
		placeNewCorridor(newCorridor,map);
		i++;
		// printf("Room 6 sentinel? %d\n",isSentinel(room[6]));
		// return; 						//For testing purposes
	}
}

struct Corridor * generateNewCorridor(struct Coordinate c1, struct Coordinate c2){
	static struct Corridor cor;
	cor.start.x = c1.x+1;
	cor.start.y = c1.y+1;
	cor.midpoint.x = c2.x+1;
	cor.midpoint.y = c1.y+1;
	cor.end.x = c2.x+1;
	cor.end.y = c2.y+1;

	printf("From (%d,%d) through (%d,%d) to (%d,%d)\n",
		cor.start.x,cor.start.y,
		cor.midpoint.x,cor.midpoint.y,
		cor.end.x,cor.end.y);

	return &cor;
}


void placeNewCorridor(struct Corridor cor, struct Map *map){
	printf("Placing corridor\n");
	int incrementer = (cor.midpoint.x - cor.start.x)/abs(cor.midpoint.x - cor.start.x);
	int curPosition = cor.start.x;
	while(curPosition != cor.midpoint.x+incrementer){
		struct Block curBlock = map -> block[curPosition][cor.midpoint.y];
		if(curBlock.type == rock){
			map -> block[curPosition][cor.midpoint.y].type = corridor;
		}
		curPosition = curPosition + incrementer;
	}


	incrementer = (cor.midpoint.y - cor.end.y)/abs(cor.midpoint.y - cor.end.y);
	curPosition = cor.end.y;
	while(curPosition != cor.midpoint.y+incrementer){
		struct Block curBlock = map -> block[cor.midpoint.x][curPosition];
		if(curBlock.type == rock){
			map -> block[cor.midpoint.x][curPosition].type = corridor;
		}
		curPosition = curPosition + incrementer;
	}
}

bool isSentinel(struct Room room){
	return room.height == -1;
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
		case rock:
			return ' ';
			break;
		case floor:
			return '.';
			break;
		case corridor:
			return '#';
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