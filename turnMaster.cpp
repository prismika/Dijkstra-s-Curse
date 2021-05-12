#include <stddef.h>
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include "turnMaster.h"
#include "heap.h"
#include "display.h"


static int32_t compare_entity_heap_entries(const void * v1, const void * v2){
	const EntityHeapEntry * e1 = (EntityHeapEntry *) v1;
	const EntityHeapEntry * e2 = (EntityHeapEntry *) v2;
	return (e1->priority == e2->priority) ?
		(e1->sequenceNumber - e2->sequenceNumber) :
		(e1->priority - e2->priority);
}

static void delete_entity_heap_entry(void * v){
	EntityHeapEntry * e = (EntityHeapEntry *) v;
	free(e);
}

void turnmaster_init(TurnMaster * tm){
	heap_init(&(tm->heap),compare_entity_heap_entries,delete_entity_heap_entry);
}


void turnmaster_fill_from_matrix(TurnMaster * tm, Entity ** matrix){
	int x,y;
	Entity * ent;
	for(y=0;y<MAPHEIGHT;y++){
		for(x=0;x<MAPWIDTH;x++){
			if(matrix[x+MAPWIDTH*y] != NULL){
				ent = matrix[x+MAPWIDTH*y];
				EntityHeapEntry * entry = (EntityHeapEntry *) malloc(sizeof(EntityHeapEntry));
				entry->entity = ent;
				entry->sequenceNumber = tm->heap.size;
				entry->priority = 0;
				entry->hn = heap_insert(&(tm->heap),entry);
			}
		}
	}
}

Entity * turnmaster_get_next_turn(TurnMaster * tm){
	EntityHeapEntry * nextEntry = (EntityHeapEntry *) heap_peek_min(&(tm->heap));
	if(nextEntry == NULL){
		std::cout << "Turnmaster is empty! No more turns!\n";
		return NULL;
	}

	Entity * nextEnt = nextEntry->entity;
	if(nextEnt->isPC){
		nextEntry->priority += 1000/(((PC*)nextEnt)->getSpeed());
	}else{
		nextEntry->priority += 1000/(((NPC*)nextEnt)->getSpeed());
	}
	heap_remove_min(&(tm->heap));
	if(!(nextEnt->dead)){
		heap_insert(&(tm->heap),nextEntry);
		return nextEnt;
	}else{
		return turnmaster_get_next_turn(tm);
	}
}