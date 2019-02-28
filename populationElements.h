#ifndef POPULATIONELEMENTS_H
#define POPULATIONELEMENTS_H
#include "mapElements.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct Entity{
	// PC *pc;
	// NPC *npc;
	Coordinate position;
	char symbol;
	int speed;
	bool dead;
	bool isPC; //Remove this later
	Coordinate (*move_strategy)(struct Entity * ent, DistanceMap * map);
} Entity;

void init_entity_npc(Entity *ent, Coordinate coord, char symbol, uint32_t characteristics);
void init_entity_pc(Entity *ent, Coordinate coord, char symbol);
void entity_get_move(Entity *ent, DistanceMap * map, Coordinate * coord);

#endif