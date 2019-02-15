
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <stddef.h>
#include "dij.h"
#include "mapElements.h"
#include "heap.h"

typedef struct heap_pair{
	heap_node_t * hn;
	int key;
	Coordinate coordinate;
}heap_pair_t;

int32_t compare_heap_pairs(const void *key, const void *with){
	heap_pair_t * p1 = (heap_pair_t *) key;
	heap_pair_t * p2 = (heap_pair_t *) with;
	return p1->key - p2->key;
}

void heap_pair_create(heap_pair_t *p, int key, int x, int y){
	Coordinate coord;
	coord.x = x;
	coord.y = y;
	heap_pair_t pair;
	pair.coordinate = coord;
	pair.key = key;
	*p = pair;
}

//For testing purposes
void displayHeap(heap_t * heap){

	heap_pair_t *p;
	while((p = heap_remove_min(heap))){
		printf("Key: %d, Coords: %d, %d\n",p->key,p->coordinate.x,p->coordinate.y);
	}
}

//Dijkstra's algorithm
int get_distance_map(Map * map, Coordinate source, DistanceMap * dist){
	int i, j;
	heap_t heap;

	//Initialize distance map to max int, but source to zero
	for(i=0; i<MAPHEIGHT; ++i){
		for(j=0; j<MAPWIDTH; ++j){
			set_distance(dist,j,i,INT_MAX);
		}
	}
	set_distance(dist,source.y,source.x,0);

	//Initialize heap; Heap elements look like (total dist,(x,y))
	heap_init(&heap,compare_heap_pairs,NULL);
	//Add source to heap
	heap_pair_t src;
	heap_pair_create(&src, 0,source.x, source.y);
	heap_insert(&heap,&src);
	displayHeap(&heap);

	//While heap is nonempty,
		//Remove point from heap, cur
		//Mark cur as processed
		//Update distance map with distance to cur
		//For each neighboring point nbr,
			//If nbr is on the border, continue
			//If nbr has been processed, continue
			//Calculate cost from cur to nbr
			//Calculate total distance to nbr
			//Add nbr to heap


	return 0;
}



