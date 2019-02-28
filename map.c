#include <stdlib.h>
#include <stdio.h>
#include "map.h"
#include "mapElements.h"
#include "populationElements.h"

char symbols[] = "0123456789abcdef";

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


	for(i = 0; i < MAPHEIGHT; ++i){
		for(j = 0; j < MAPWIDTH; ++j){
			retMap.populationMap[i][j] = NULL;
		}
	}

	distance_map_init(&retMap.distanceMapNonTunneling);
	distance_map_init(&retMap.distanceMapTunneling);

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
void map_set_entity(Map * map, int x, int y, Entity * ent){
	map->populationMap[y][x] = ent;
	Coordinate coord;
	coord.x = x;
	coord.y = y;
	ent->position = coord;
}
void map_remove_entity(Map * map, int x, int y){
	map->populationMap[y][x] = NULL;
}
Coordinate map_move_entity(Map * map, Entity * ent, Coordinate target){
	Coordinate position = ent->position;
	Block targetBlock;
	map_getBlock(map, target.x,target.y, &targetBlock);
	//If target is soft rock, break it (it'll do the next "if" too)
	if(targetBlock.type == rock && targetBlock.hardness <= 85){
		targetBlock.hardness = 0;
		targetBlock.type = corridor;
		map_setBlock(map, target.x, target.y, &targetBlock);
	}
	//If target is open, move there (and kill whatever is there)
	if(targetBlock.hardness == 0){
		if(map_has_entity_at(map, target.x,target.y)){
			Entity ent;
			map_get_entity(map, target.x, target.y, &ent);
			ent.dead = true;
		}
		map_remove_entity(map, position.x,position.y);
		map_set_entity(map, target.x, target.y, ent);
		return target;
	//If target is hard rock, soften it
	}else if(targetBlock.type == rock){
		targetBlock.hardness -= 85;
		map_setBlock(map, target.x, target.y, &targetBlock);
	}
	return position;
}
Entity ** map_get_population_matrix(Map * map){
	return &(map->populationMap[0][0]);
}
void map_new_npc(Map * map, Coordinate coord, uint32_t characteristics){
	Entity * ent = malloc(sizeof(Entity));
	char symbol = symbols[characteristics];
	init_entity_npc(ent,coord,symbol,characteristics);
	map_set_entity(map, coord.x, coord.y, ent);
}
void map_new_pc(Map * map, Coordinate coord){
	Entity * ent = malloc(sizeof(Entity));
	init_entity_pc(ent,coord,'@');
	map_set_entity(map, coord.x, coord.y, ent);
}
bool map_has_entity_at(Map * map, int x, int y){
	return !(map->populationMap[y][x] == NULL);
}
void map_get_entity(Map * map, int x, int y, Entity * ent){
	*ent = *(map->populationMap[y][x]);
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

DistanceMap * map_get_distance_map_non_tunneling(Map * map){
	return &(map->distanceMapNonTunneling);
}
DistanceMap * map_get_distance_map_tunneling(Map * map){
	return &(map->distanceMapTunneling);
}
