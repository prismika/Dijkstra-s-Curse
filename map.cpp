#include <cstdlib>
#include <iostream>
#include <string>
#include "map.h"
#include "mapElements.h"
#include "populationElements.h"
#include "display.h"

#define LIGHT_GLOW 3

void map_init(Map * map){
	Map retMap;
	Block default_remembered_block = block_create(rock,100);
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
			map_set_block_remembered(&retMap, j, i, &default_remembered_block);
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
			retMap.itemMap[i][j] = NULL;
			// retMap.isVisible[i][j] = false;
		}
	}
	retMap.populationList = NULL;
	retMap.populationListSize = 0;

	distance_map_init(&retMap.distanceMapNonTunneling);
	distance_map_init(&retMap.distanceMapTunneling);

	retMap.bossIsDead = false;

	*map = retMap;
}

int map_getBlock(Map * map, int x, int y, Block * outputBlock){
	if(x<0||x>=MAPWIDTH||y<0||y>=MAPHEIGHT){
		std::cerr << "!!!Tried to get block in map at ("<< x << "," << y << ")!!!\n";
		return -1;
	}
	*outputBlock = map->block[y][x];
	return 0;
}
int map_setBlock(Map * map, int x, int y, Block * inputBlock){
	if(x<0||x>=MAPWIDTH||y<0||y>=MAPHEIGHT){
		std::cerr << "!!!Tried to set block in map at ("<< x << "," << y << ")!!!\n";
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
Entity ** map_get_population_matrix(Map * map){
	return &(map->populationMap[0][0]);
}
Entity ** map_get_population_list(Map * map){
	return map->populationList;
}

int map_get_population_size(Map * map){
	return map->populationListSize;
}

Coordinate map_get_pc_position(Map * map){
	return map->populationList[0]->position;
}

PC * map_get_pc(Map * map){
	return (PC*)map->populationList[0];
}

bool map_has_entity_at(Map * map, int x, int y){
	return !(map->populationMap[y][x] == NULL);
}
bool map_has_item_at(Map * map, int x, int y){
	return !(map->itemMap[y][x] == NULL);
}
void map_get_entity(Map * map, int x, int y, Entity * ent){
	*ent = *(map->populationMap[y][x]);
}
static Entity * map_get_entity_address(Map * map, int x, int y){
	return map->populationMap[y][x];
}
void map_kill_entity(Map * map, Coordinate coord){
	Entity * targetEnt = map_get_entity_address(map, coord.x, coord.y);
	targetEnt->dead = true;
	if(!targetEnt->isPC){
		map->bossIsDead = ((NPC*) targetEnt)->isBoss() || map->bossIsDead;
	}
	map_remove_entity(map, coord.x, coord.y);
}

//Returns position of entity after move is attempted
//This is a god function
Coordinate map_move_entity(Map * map, Entity * ent, Coordinate target){
	Coordinate position = ent->position;
	//If entity isn't even trying to move, we're done
	if(position.x == target.x && position.y == target.y){
		return target;
	}
	Block targetBlock;
	map_getBlock(map, target.x,target.y, &targetBlock);
	//If it can't tunnel, don't let it tunnel
	if(!ent->canTunnel && targetBlock.hardness > 0) return position;
	//If target is soft rock, break it (it'll do the next "if" block too)
	if(targetBlock.type == rock && targetBlock.hardness <= 85){
		targetBlock.hardness = 0;
		targetBlock.type = corridor;
		map_setBlock(map, target.x, target.y, &targetBlock);
	}
	//If target is open, move there (and kill whatever is there)
	if(targetBlock.hardness == 0){
		if(map_has_entity_at(map, target.x,target.y)){
			Entity * targetEnt = map->populationMap[target.y][target.x];
			if(ent->isPC ^ targetEnt->isPC){
				int damage = ent->attack(targetEnt);
				if(targetEnt->dead){
					if(ent -> isPC){
						string msg = "You killed a ";
						msg += *targetEnt->getName();
						msg += " by doing ";
						msg += to_string(damage);
						msg += " damage";
						display_message(msg.c_str());
					}
					map_kill_entity(map,target);
				}else{
					if(ent->isPC){
						string msg = "You hit a ";
						msg += *targetEnt->getName();
						msg += " for ";
						msg += to_string(damage);
						msg += " damage";
						display_message(msg.c_str());
					}
					return position;
				}
			}else{
				//NPC on NPC crime. Swap positions.
				map_remove_entity(map, position.x,position.y);
				map_remove_entity(map, target.x,target.y);
				map_set_entity(map, target.x, target.y, ent);
				map_set_entity(map, position.x, position.y, targetEnt);
				return target;
			}
		}
		map_remove_entity(map, position.x,position.y);
		map_set_entity(map, target.x, target.y, ent);
		//The pc picks up whatever item might be there
		if(ent->isPC
			&& map_has_item_at(map, target.x, target.y)){
			//If PC accepted the item, remove it from the floor
			if(((PC*)ent)->giveItem(map->itemMap[target.y][target.x])
				!= -1){
				map->itemMap[target.y][target.x]=NULL;
			}
		}
		return target;
	//If target is hard rock, soften it
	}else if(targetBlock.type == rock){
		targetBlock.hardness -= 85;
		map_setBlock(map, target.x, target.y, &targetBlock);
	}
	return position;
}
//Unconditional teleport
int map_teleport_entity(Map * map, Entity * ent, Coordinate target){
	Block targetBlock;
	map_getBlock(map, target.x, target.y, &targetBlock);
	if(targetBlock.hardness > 0){
		targetBlock.hardness = 0;
		targetBlock.type = floor;
		map_setBlock(map, target.x, target.y, &targetBlock);
	}
	//Move entity
	Coordinate start = ent->position;
	map_remove_entity(map, start.x, start.y);
	map_set_entity(map, target.x, target.y, ent);
	return 0;
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

bool map_pc_is_dead(Map * map){
	return map->populationList[0]->dead;
}

DistanceMap * map_get_distance_map_non_tunneling(Map * map){
	return &(map->distanceMapNonTunneling);
}
DistanceMap * map_get_distance_map_tunneling(Map * map){
	return &(map->distanceMapTunneling);
}

bool map_block_is_visible(Map * map, Coordinate targetCoord){
	PC * pc = map_get_pc(map);
	Coordinate pcCoord = map_get_pc_position(map);
	int dist = distLInf(targetCoord,pcCoord);
	if(!map->checkLOS(pcCoord, targetCoord)) return false;
	if(dist <= pc->getGlow()) return true;

	//TODO This is terrible from a performance perspective
	for(int y = 0; y < MAPHEIGHT; y++){
		for(int x = 0; x < MAPWIDTH; x++){
			if(map_has_item_at(map, x, y)){
				Item *item = map->getItemAt(x,y);
				if(item->getType() == ITEM_TYPE_LIGHT){
					Coordinate curCoord;
					curCoord.x = x;
					curCoord.y = y;
					int dist = distLInf(curCoord, targetCoord);
					// if(dist <= item->getAttribute()
					// 	&& map->checkLOS(blah blah)) return true;
					if(dist <= LIGHT_GLOW) return true;
				}
			}
		}
	}
	// bool closeEnough = distLInf(targetCoord, pcCoord) <= pc->getGlow();
	// if(closeEnough){
	// 	return map->checkLOS(pcCoord, targetCoord);
	// }else{
	// 	return false;
	// }
	return false;
}


int map_get_block_remembered(Map * map, int x, int y, Block * outputBlock){
	if(x<0||x>=MAPWIDTH||y<0||y>=MAPHEIGHT){
		std::cerr << "!!!Tried to get remembered block in map at ("<< x << "," << y << ")!!!\n";
		return -1;
	}
	*outputBlock = map->block_remembered[y][x];
	return 0;
}

int map_set_block_remembered(Map * map, int x, int y, Block * inputBlock){
	if(x<0||x>=MAPWIDTH||y<0||y>=MAPHEIGHT){
		std::cerr << "!!!Tried to set remembered block in map at ("<< x << "," << y << ")!!!\n";
		return -1;
	}
	map->block_remembered[y][x] = *inputBlock;
	return 0;
}

int map_update_remembered(Map * map){
	int x,y;
	for(y = 0; y < MAPHEIGHT; y++){
		for(x = 0; x < MAPWIDTH; x++){
			Coordinate curCoord;
			curCoord.x = x;
			curCoord.y = y;
			if(map_block_is_visible(map, curCoord)){
				Block realBlock;
				map_getBlock(map, x, y, &realBlock);
				map_set_block_remembered(map, x, y, &realBlock);
			}
		}
	}
	return 0;
}

Item * Map::getItemAt(Coordinate coord){
	return itemMap[coord.y][coord.x];
}

Item * Map::getItemAt(int x, int y){
	return itemMap[y][x];
}

int Map::placeItem(Item * item, Coordinate coord){
	itemMap[coord.y][coord.x] = item;
	return 0;
}

bool Map::isBossDead(){
	return this->bossIsDead;
}

/*Bresenham's Algorithm*/
bool Map::checkLOS(Coordinate target, Coordinate source){
	if(source.x == target.x && source.y == target.y) return true;
	int deltaX = target.x - source.x;
	int deltaY = target.y - source.y;
	Coordinate curCoord = source;
	if(abs(deltaX) > abs(deltaY)){
		float deltaError = deltaY/(float)deltaX;
		float error = 0;
		int deltaYInc = (deltaY>0)? 1 : -1;
		int curCoordXInc = (deltaX>0)? 1 : -1;
		int errorInc = (deltaError>0)? -1 : 1;
		for(;curCoord.x != target.x; curCoord.x+= curCoordXInc){
			if(this->obstructionAt(curCoord)){
				return false;
			}
			error += deltaError;
			if(abs(error) >= 0.5){
				curCoord.y += deltaYInc;
				error += errorInc;
			}
		}
	}else{
		float deltaError = deltaX/(float)deltaY;
		float error = 0;
		int deltaXInc = (deltaX>0)? 1 : -1;
		int curCoordYInc = (deltaY>0)? 1 : -1;
		int errorInc = (deltaError>0)? -1 : 1;
		for(; curCoord.y != target.y; curCoord.y+= curCoordYInc){
			if(this->obstructionAt(curCoord)){
				return false;
			}
			error += deltaError;
			if(abs(error) >= 0.5){
				curCoord.x += deltaXInc;
				error += errorInc;
			}
		}
	}
	return true;
}

bool Map::obstructionAt(Coordinate coord){
	return this->block[coord.y][coord.x].hardness > 0;
}
