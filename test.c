#include <stdlib.h>
#include <stdio.h>
#include "heap.h"
#include "mapElements.h"
#include "map.h"
#include "populationElements.h"
#include "display.h"
#include "mapBuilder.h"
#include "mapPopulator.h"
typedef struct heap_pair{
	heap_node_t * hn;
	int key;
	int goods;
}heap_pair_t;

int32_t cmp(const void *key, const void *with){
	heap_pair_t * p1 = (heap_pair_t *) key;
	heap_pair_t * p2 = (heap_pair_t *) with;
	return p1->key - p2->key;
}



void funfunkyfunc(Map * theMap, Coordinate coord){
	map_new_npc(theMap, coord, 1);

}

int main(void){
	// heap_t heap;
	// heap_init(&heap, cmp, NULL);
	// heap_pair_t newPair;
	// newPair.key = 10;
	// newPair.goods = 1000;
	// newPair.hn = heap_insert(&heap,&newPair);
	// heap_pair_t newPair2;
	// newPair2.key = 2;
	// newPair2.goods = 200;
	// newPair2.hn = heap_insert(&heap,&newPair2);
	// heap_pair_t newPair3;
	// newPair3.key = 1;
	// newPair3.goods = 100;
	// newPair3.hn = heap_insert(&heap,&newPair3);
	// heap_pair_t newPair4;
	// newPair4.key = 5;
	// newPair4.goods = 0;
	// newPair4.hn = heap_insert(&heap,&newPair4);
	// newPair4.key = 0;
	// heap_decrease_key_no_replace(&heap, newPair4.hn);
	// heap_pair_t * result = (heap_pair_t *) heap_remove_min(&heap);
	// printf("%d\n", result->goods);
	// result = (heap_pair_t *) heap_remove_min(&heap);
	// printf("%d\n", result->goods);
	// result = (heap_pair_t *) heap_remove_min(&heap);
	// printf("%d\n", result->goods);
	// result = (heap_pair_t *) heap_remove_min(&heap);
	// printf("%d\n", result->goods);


	Map theMap;
	map_init(&theMap);
	generate_map(&theMap,13);
	populate_map(&theMap,5);
	display_map(&theMap);
	display_population(&theMap);

	return 0;
}

