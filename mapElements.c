#include "mapElements.h"
#include <stdlib.h>
#include <stdio.h>



struct Block block_create(enum BlockType blockType, uint8_t hardness){
	struct Block newBlock;
	newBlock.type = blockType;
	newBlock.hardness = hardness;
	return newBlock;
}
void map_init(Map * map){
	Map retMap;
	//init room array
	map = &retMap;
}
//TODO check bounds on this and setBlock
int map_getBlock(Map * map, int x, int y, Block * outputBlock){
	*outputBlock = map->block[y][x];
	return 0;
}
int map_setBlock(Map * map, int x, int y, Block * inputBlock){
	map->block[y][x] = *inputBlock;
	return 0;
}
int get_distance(DistanceMap * dist, int x, int y){
	return dist->dist[y][x];
}

void set_distance(DistanceMap * dist, int x,int y,int d){
	dist->dist[y][x] = d;
}
