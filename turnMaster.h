#ifndef TURNMASTER_H
#define TURNMASTER_H

#include "populationElements.h"
#include "mapElements.h"
#include "heap.h"

typedef struct{
	heap_node_t * hn;
	Entity * entity;
	int sequenceNumber;
	int priority;
}EntityHeapEntry;

typedef struct{
	heap_t heap;
}TurnMaster;



void turnmaster_init(TurnMaster * tm);
void turnmaster_add_entity(TurnMaster * tm, Entity * ent);
void turnmaster_fill_from_matrix(TurnMaster * tm, Entity ** matrix);
Entity * turnmaster_get_next_turn(TurnMaster * tm);





#endif