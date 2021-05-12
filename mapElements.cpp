#include "mapElements.h"
#include <stdlib.h>
#include <iostream>
#include <stdbool.h>

Block block_create(enum BlockType blockType, uint8_t hardness){
	Block newBlock;
	newBlock.type = blockType;
	newBlock.hardness = hardness;
	return newBlock;
}


void distance_map_init(DistanceMap * dist){
	int x,y;
	for(y=0; y<MAPHEIGHT; y++){
		for(x=0; x<MAPHEIGHT; x++){
			dist-> dist[y][x] = 0;
		}
	}
}

int get_distance(DistanceMap * dist, int x, int y){
	if(x<0||x>=MAPWIDTH||y<0||y>=MAPHEIGHT){
		std::cout << "!!!Tried to get distance from (%" << x << "," << x << ")!!!\n";
		return -1;
	}
	return dist->dist[y][x];
}

void set_distance(DistanceMap * dist, int x,int y,int d){
	if(x<0||x>=MAPWIDTH||y<0||y>=MAPHEIGHT){
		std::cout << "!!!Tried to set distance from (%" << x << "," << x << ")!!!\n";
	}
	dist->dist[y][x] = d;
}

bool room_is_sentinel(Room room){
	return room.height == -1;
}

//L-infinity metric
int distLInf(Coordinate v1, Coordinate v2){
	return (abs(v1.x-v2.x)<abs(v1.y-v2.y)) ?
		abs(v1.y-v2.y) :
		abs(v1.x-v2.x);
}
