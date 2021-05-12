#ifndef TURNMASTER_H
#define TURNMASTER_H

#include "populationElements.h"
#include "mapElements.h"
#include "heap.h"

class EntityHeapEntry{
public:
	heap_node_t * hn;
	Entity * entity;
	int sequenceNumber;
	int priority;
};

class TurnMaster{
public:
	heap_t heap;
};



void turnmaster_init(TurnMaster * tm);
void turnmaster_add_entity(TurnMaster * tm, Entity * ent);
void turnmaster_fill_from_matrix(TurnMaster * tm, Entity ** matrix);
Entity * turnmaster_get_next_turn(TurnMaster * tm);





#endif