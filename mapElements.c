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
	int i,j;
	for(i = 0; i < MAPHEIGHT; ++i){
		for(j = 0; j < MAPWIDTH; ++j){
			Block block;
			if((i==0||i==MAPHEIGHT-1)||(j==0||j==MAPWIDTH-1)){
				block = block_create(bedrock,255);
			}else{
				block = block_create(rock,100);
			}
			map_setBlock(&retMap, j, i, &block);
		}
	}

	for(i=0;i<MAX_ROOM_COUNT+1;i++){
		Room sentinelRoom;
		sentinelRoom.height=-1;
		sentinelRoom.width=-1;
		sentinelRoom.position.x = 0;
		sentinelRoom.position.y = 0;
		retMap.room[i] = sentinelRoom;
	}

	retMap.pcPos.x = 1;
	retMap.pcPos.y = 1;

	*map = retMap;
}
//TODO check bounds on this and setBlock
int map_getBlock(Map * map, int x, int y, Block * outputBlock){
	if(x<0||x>=MAPWIDTH||y<0||y>=MAPHEIGHT){
		fprintf(stderr, "!!!Tried to get block from map at (%d,%d)!!!\n",x,y);
		return -1;
	}
	*outputBlock = map->block[y][x];
	return 0;
}
int map_setBlock(Map * map, int x, int y, Block * inputBlock){
	if(x<0||x>=MAPWIDTH||y<0||y>=MAPHEIGHT){
		fprintf(stderr, "!!!Tried to set block in map at (%d,%d)!!!\n",x,y);
		return -1;
	}
	map->block[y][x] = *inputBlock;
	return 0;
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
