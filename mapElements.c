#include "mapElements.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct Block block_create(enum BlockType blockType, uint8_t hardness){
	struct Block newBlock;
	newBlock.type = blockType;
	newBlock.hardness = hardness;
	return newBlock;
}

int get_distance(DistanceMap * dist, int x, int y){
	if(x<0||x>=MAPWIDTH||y<0||y>=MAPHEIGHT){
		fprintf(stderr, "!!!Tried to get distance from (%d,%d)!!!\n",x,y);
		return -1;
	}
	return dist->dist[y][x];
}

void set_distance(DistanceMap * dist, int x,int y,int d){
	if(x<0||x>=MAPWIDTH||y<0||y>=MAPHEIGHT){
		fprintf(stderr, "!!!Tried to set distance from (%d,%d)!!!\n",x,y);
	}
	dist->dist[y][x] = d;
}



bool room_is_sentinel(Room room){
	return room.height == -1;
}