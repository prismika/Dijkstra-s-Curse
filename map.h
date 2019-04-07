#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include "mapElements.h"
#include "populationElements.h"


class Map{
public:
	Block block[MAPHEIGHT][MAPWIDTH];
	Block block_remembered[MAPHEIGHT][MAPWIDTH];
	Room room[MAX_ROOM_COUNT+1];//DONT FORGET extra spot for sentinel value
	int width;
	int height;
	Entity * populationMap[MAPHEIGHT][MAPWIDTH];
	Entity ** populationList;
	int populationListSize;
	DistanceMap distanceMapNonTunneling;
	DistanceMap distanceMapTunneling;
};

void map_init(Map * map);
int map_getBlock(Map * map, int x, int y, Block * outputBlock);
int map_setBlock(Map * map, int x, int y, Block * inputBlock);
void map_change_block_type(Map * map, int x, int y, BlockType type);
NPC * map_new_npc(Map * map, Coordinate coord, uint32_t characteristics);
Entity * map_new_pc(Map * map, Coordinate coord);
void map_set_entity(Map * map, int x, int y, Entity * ent);
//Performs all necessary checks and changes to attempt the specified move
Coordinate map_move_entity(Map * map, Entity * ent, Coordinate coord);
//Unconditionally clears a space and teleports the entity
int map_teleport_entity(Map * map, Entity * ent, Coordinate target);
Entity ** map_get_population_matrix(Map * map);
Entity ** map_get_population_list(Map * map);
int map_get_population_size(Map * map);
Coordinate map_get_pc_position(Map * map);
Entity * map_get_pc(Map * map);
bool map_has_entity_at(Map * map, int x, int y);
void map_get_entity(Map * map, int x, int y, Entity * ent);
void map_choose_random_block(Map *map, enum BlockType canChoose[], int canChooseSize, Coordinate *returnCoord);
bool map_pc_is_dead(Map * map);
DistanceMap * map_get_distance_map_non_tunneling(Map * map);
DistanceMap * map_get_distance_map_tunneling(Map * map);
bool map_block_is_visible(Map * map, Coordinate coord);
int map_get_block_remembered(Map * map, int x, int y, Block * outputBlock);
int map_set_block_remembered(Map * map, int x, int y, Block * inputBlock);
int map_update_remembered(Map * map);

#endif