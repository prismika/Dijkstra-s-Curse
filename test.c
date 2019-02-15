#include <stdlib.h>
#include <stdio.h>
#include "heap.h"
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

int main(void){
	heap_t heap;
	heap_init(&heap, cmp, NULL);
	heap_pair_t newPair;
	newPair.key = 10;
	newPair.goods = 1000;
	newPair.hn = heap_insert(&heap,&newPair);
	heap_pair_t newPair2;
	newPair2.key = 5;
	newPair2.goods = 200;
	newPair2.hn = heap_insert(&heap,&newPair2);
	heap_pair_t newPair3;
	newPair3.key = 1;
	newPair3.goods = 100;
	newPair3.hn = heap_insert(&heap,&newPair3);
	heap_pair_t newPair4;
	newPair4.key = 5;
	newPair4.goods = 500;
	newPair4.hn = heap_insert(&heap,&newPair4);
	heap_decrease_key_no_replace(&heap, newPair4.hn);
	heap_pair_t * result = (heap_pair_t *) heap_remove_min(&heap);
	printf("%d\n", result->goods);
	result = (heap_pair_t *) heap_remove_min(&heap);
	printf("%d\n", result->goods);
	result = (heap_pair_t *) heap_remove_min(&heap);
	printf("%d\n", result->goods);
	result = (heap_pair_t *) heap_remove_min(&heap);
	printf("%d\n", result->goods);



	return 0;
}

