#ifndef POPULATIONELEMENTS_H
#define POPULATIONELEMENTS_H
#include "mapElements.h"
#include <stdint.h>

typedef struct{
	// PC *pc;
	// NPC *npc;
	Coordinate position;
	char symbol;
} Entity;

void init_entity_npc(Entity *ent, Coordinate coord, char symbol, uint32_t characteristics);



#endif