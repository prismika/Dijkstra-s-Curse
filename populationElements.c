#include "populationElements.h"

#include "mapElements.h"

Coordinate tunnelMove(Entity * ent, DistanceMap * map){
	Coordinate ret;
	ret.x = ent->position.x+1;
	ret.y = ent->position.y;
	return ret;
}

void init_entity_npc(Entity *ent, Coordinate coord, char symbol, uint32_t characteristics){
	ent->position = coord;
	ent->symbol = symbol;
	ent->dead = false;
	ent->speed = 5;
	ent->isPC = false;
	//end->pc = NULL;
	//ent->npc = &npc;
	switch(characteristics){
		default:
		ent->move_strategy = tunnelMove;
	}
}

void init_entity_pc(Entity *ent, Coordinate coord, char symbol){
	ent->position = coord;
	ent->symbol = symbol;
	ent->dead = false;
	ent-> speed = 5;
	ent-> isPC = true;
	//end->pc = &pc;
	//ent->npc = NULL;
	ent->move_strategy = tunnelMove;
}

void entity_get_move(Entity *ent, DistanceMap * map, Coordinate * coord){
	*coord = ent->move_strategy(ent, map);
}
