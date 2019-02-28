#ifndef POPULATIONELEMENTS_H
#define POPULATIONELEMENTS_H
#include "mapElements.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct{
	// PC *pc;
	// NPC *npc;
	Coordinate position;
	char symbol;
	int speed;
	bool dead;
} Entity;

void init_entity_npc(Entity *ent, Coordinate coord, char symbol, uint32_t characteristics);
void init_entity_pc(Entity *ent, Coordinate coord, char symbol);

#endif