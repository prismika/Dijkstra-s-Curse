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
void map_change_block_type(Map * map, int x, int y, BlockType type){
	Block blockToChange;
	map_getBlock(map,x,y,&blockToChange);
	blockToChange.type = type;
	map_setBlock(map,x,y,&blockToChange);
}
void map_choose_random_block(Map *map, enum BlockType canChoose[], int canChooseSize, Coordinate *returnCoord){
	while(true){
		int yCoord = rand()%MAPHEIGHT;
		int xCoord = rand()%MAPWIDTH;
		Block curBlock;
		map_getBlock(map,xCoord,yCoord,&curBlock);

		//check if it is permissible to choose curBlock
		int i;
		for(i=0;i<canChooseSize; ++i){
			if(canChoose[i] == curBlock.type){
				Block newTypeBlock;
				map_getBlock(map,xCoord,yCoord,&newTypeBlock);
				returnCoord->x = xCoord;
				returnCoord->y = yCoord;
				return;
			}
		}
		//The chosen block was of a type not in the canChoose list.
	}
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
