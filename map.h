#ifndef MAP_H
#define MAP_H

#include "mapElements.h"

typedef struct Map{
	Block block[MAPHEIGHT][MAPWIDTH];
	Room room[MAX_ROOM_COUNT+1];//DONT FORGET extra spot for sentinel value
	Coordinate pcPos;
	int width;
	int height;
} Map;

void map_init(Map * map);
int map_getBlock(Map * map, int x, int y, Block * outputBlock);
int map_setBlock(Map * map, int x, int y, Block * inputBlock);
void map_change_block_type(Map * map, int x, int y, BlockType type);
void map_choose_random_block(Map *map, enum BlockType canChoose[], int canChooseSize, Coordinate *returnCoord);


#endif