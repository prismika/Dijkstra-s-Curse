#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "turnMaster.h"
#include "heap.h"


static int32_t compare_entity_heap_entries(const void * e1, const void * e2){
	return 0;
}

static void delete_entity_heap_entry(void * v){
	EntityHeapEntry * e = v;
	free(e);
}

void turnmaster_init(TurnMaster * tm){
	heap_init(&(tm->heap),compare_entity_heap_entries,delete_entity_heap_entry);
}


void turnmaster_add_entity(TurnMaster * tm, Entity * ent){
	EntityHeapEntry * entry = malloc(sizeof(EntityHeapEntry));
	entry->entity = ent;
	entry->sequenceNumber = tm->heap.size;
	entry->priority = 0;
	printf("Adding Entity to Turnmaster: %c, %d, %d\n",
		ent->symbol, ent->position.x, ent->position.y);
	entry->hn = heap_insert(&(tm->heap),entry);
}

void turnmaster_fill_from_matrix(TurnMaster * tm, Entity ** matrix){
	int x,y;
	for(y=0;y<MAPHEIGHT;y++){
		for(x=0;x<MAPWIDTH;x++){
			if(matrix[x+MAPWIDTH*y]!=NULL){
				turnmaster_add_entity(tm, matrix[x+MAPWIDTH*y]);
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
	printf("Returning %c at %d, %d\n", nextEnt->symbol, nextEnt->position.x, nextEnt->position.y);
	return nextEnt;
}