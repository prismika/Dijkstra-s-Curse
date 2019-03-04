#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "turnMaster.h"
#include "heap.h"
#include "display.h"


static int32_t compare_entity_heap_entries(const void * v1, const void * v2){
	const EntityHeapEntry * e1 = v1;
	const EntityHeapEntry * e2 = v2;
	return (e1->priority == e2->priority) ?
		(e1->sequenceNumber - e2->sequenceNumber) :
		(e1->priority - e2->priority);
}

static void delete_entity_heap_entry(void * v){
	EntityHeapEntry * e = v;
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
				EntityHeapEntry * entry = malloc(sizeof(EntityHeapEntry));
				entry->entity = ent;
				entry->sequenceNumber = tm->heap.size;
				entry->priority = 0;
				// printf("Adding Entity to Turnmaster: %c, %d, %d\n",
				// 	ent->symbol, ent->position.x, ent->position.y);
				entry->hn = heap_insert(&(tm->heap),entry);
			}
		}
	}
}

Entity * turnmaster_get_next_turn(TurnMaster * tm){
	EntityHeapEntry * nextEntry = heap_peek_min(&(tm->heap));
	if(nextEntry == NULL){
		printf("Turnmaster is empty! No more turns!\n");
		return NULL;
	}

	Entity * nextEnt = nextEntry->entity;
	nextEntry->priority += 1000/(nextEnt->speed);
	heap_remove_min(&(tm->heap));
	// printf("Removing from heap: ");
	// display_entity(nextEnt);
	// printf("Heap size is now %d\n", tm->heap.size);
	if(!(nextEnt->dead)){
		heap_insert(&(tm->heap),nextEntry);
		return nextEnt;
		// printf("Returning %c at (%d, %d), new priority %d\n",
		// nextEnt->symbol, nextEnt->position.x, nextEnt->position.y, nextEntry->priority);
	}else{
		return turnmaster_get_next_turn(tm);
	}
}