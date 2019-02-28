#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "map.h"
#include "mapBuilder.h"
#include "mapElements.h"

#define FAILED_ROOM_ATTEMPTS_LIMIT 32

#define SECOND_CLOSEST_ROOM_CONNECTION_ODDS/*One in...*/ 3

void populateWithRooms(Map *map);
Room generateNewRoom(Map *map);
int isLegalRoom(Room *room, Map *map);
void placeNewRoom(Map *map, Room *room);
void populateWithCorridors(Map *map);
int dist(Room *r1, Room *r2);
Corridor * generateNewCorridor(Coordinate c1, Coordinate c2);
void placeNewCorridor(Corridor cor, Map *map);
void placePartialCorridor(Coordinate origin, int dist, bool horizontal, Map *map);

void populateWithStairs(Map *map);
void populateWithPC(Map * map);

void generate_map(Map * map, long seed){
	srand(seed);
	populateWithRooms(map);
	populateWithCorridors(map);
	populateWithStairs(map);
	populateWithPC(map);
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
			// display_map(*map);
			failures = 0;
			roomCount = 0;
			map_init(map);
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
	// display_room_list(map->room);
	int i=1;
	while(!room_is_sentinel(map->room[i])){
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


//--------------------------STAIRS------------------------------

void populateWithStairs(Map *map){
	enum BlockType 	canReplace[] 	= {floor,corridor};
	int 			canReplaceSize	= 2;

	Coordinate upStairCoord;
	Coordinate downStairCoord;
	map_choose_random_block(map,canReplace,canReplaceSize,&upStairCoord);
	map_choose_random_block(map,canReplace,canReplaceSize,&downStairCoord);
	map_change_block_type(map,upStairCoord.x,upStairCoord.y,upstairs);
	map_change_block_type(map,downStairCoord.x,downStairCoord.y,downstairs);
}

//-----------------------------Entities-------------------------
void populateWithPC(Map * map){
	Coordinate coordForPC;
	BlockType 	canChoose 		= {floor};
	int 		canChooseSize 	= 1;
	map_choose_random_block(map,&canChoose,canChooseSize,&coordForPC);
	map_new_pc(map, coordForPC);
}

