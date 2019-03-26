#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include "pathFinder.h"
#include "mapElements.h"
#include "heap.h"

class heap_pair_t{
public:
	heap_node_t * hn;
	int key;
	Coordinate coordinate;
};

int32_t compare_heap_pairs(const void *key, const void *with){
	heap_pair_t * p1, * p2;
	p1 = (heap_pair_t *) key;
	p2 = (heap_pair_t *) with;
	// printf("Comparing points (%d,%d) and (%d,%d)\n",
		// p1->coordinate.x, p1->coordinate.y,
		// p2->coordinate.x, p2->coordinate.y);
	// if(p1->key != p2->key){
	// 	printf("!!!!! Returning nonzero: distances %d and %d\n", p1->key,p2->key);
	// }
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



int get_neighborhood(int curX, int curY, Coordinate neighbors[9]){
	int i,j;
	for(i=0;i<3;++i){
		for(j=0;j<3;++j){
			Coordinate newCoord;
			newCoord.x = curX+j-1;
			newCoord.y = curY+i-1;
			neighbors[3*i+j] = newCoord;
		}
	}
	return 0;
}

int displayDistanceMap(DistanceMap * dist){
	int i, j;
	for(i=0; i<MAPHEIGHT; ++i){
		for(j=0; j<MAPWIDTH; ++j){
			int d = get_distance(dist,j,i);
			if(d==-1) printf(" ");
			else{
				printf("%d",d % 10);
			}
		}
		printf("\n");
	}
	return 0;
}



//Dijkstra's algorithm
int get_distance_map_hidden(Map * map, Coordinate source, DistanceMap * dist, int tunnelingHardnessMax){
	int i, j;
	heap_t heap;
	heap_init(&heap,compare_heap_pairs,NULL);
	heap_pair_t pairs[MAPHEIGHT][MAPWIDTH];
	bool processed[MAPHEIGHT][MAPWIDTH];

	//TODO set distance matrix at the end. Use pairs matrix instead
	//Initialize distance map to max int, but source to zero
	for(i=0; i<MAPHEIGHT; ++i){
		for(j=0; j<MAPWIDTH; ++j){
			set_distance(dist,j,i,INT_MAX);
		}
	}
	set_distance(dist,source.x,source.y,0);

	//Initialize heap pairs and fill heap
	for(i=0; i < MAPHEIGHT; ++i){
		for(j=0; j < MAPWIDTH; ++j){
			heap_pair_create(&(pairs[i][j]),INT_MAX, j, i);
			pairs[i][j].hn = heap_insert(&heap,&(pairs[i][j]));
		}
	}
	pairs[source.y][source.x].key = 0;
	heap_decrease_key_no_replace(&heap,pairs[source.y][source.x].hn);

	//Initialize processed matrix
	for(i=0; i < MAPHEIGHT; ++i){
		for(j=0; j < MAPWIDTH; ++j){
			processed[i][j] = false;
		}
	}

	//While heap is nonempty, remove point from heap, cur
	heap_pair_t * cur;
	while((cur = (heap_pair_t *)heap_remove_min(&heap))){
		int curX = cur->coordinate.x;
		int curY = cur->coordinate.y;
		int curDist = get_distance(dist,curX,curY);
		Block curBlock;
		map_getBlock(map, curX, curY, &curBlock);
		// printf("Removed from heap: --\tX|%d, Y|%d, D|%d, Size|%d --\n",
		// 	curX,curY,curDist,heap.size);
		//Mark cur as processed
		processed[curY][curX] = true;
		//For each neighboring point nbr,
		Coordinate neighbors[9];
		get_neighborhood(curX,curY,neighbors);
		for(i=0;i<9; ++i){
			Coordinate nbr = neighbors[i];
			//If nbr is out of bounds, continue
			if(nbr.y<0||nbr.y>=MAPHEIGHT||nbr.x<0||nbr.x>=MAPWIDTH){
				continue;
			}
			//If nbr is immutable, continue
			Block nbrBlock;
			map_getBlock(map,nbr.x,nbr.y,&nbrBlock);
			if(nbrBlock.hardness > tunnelingHardnessMax){
				continue;
			}
			//If nbr has been processed, continue
			if(processed[nbr.y][nbr.x]){
				continue;
			}
			//Calculate cost from cur to nbr
			int nbrCost = 1+ (curBlock.hardness / 85);
			//Calculate total distance to nbr
			int nbrDist = curDist + nbrCost;
			//If new distance is less than old one,
			if(nbrDist < get_distance(dist,nbr.x,nbr.y)){
				//Update distance map with distance to nbr
				set_distance(dist,nbr.x,nbr.y,nbrDist);
				//Update nbr distance in the heap
				pairs[nbr.y][nbr.x].key = nbrDist;
				heap_decrease_key_no_replace(&heap,pairs[nbr.y][nbr.x].hn);
				// printf("Inserted into heap: ++\tX|%d, Y|%d, D|%d, Size|%d ++\n",
				// 	nbr.x,nbr.y,nbrDist, heap.size);
			}
		}
	}
	heap_delete(&heap);

	return 0;
}

int get_distance_map_tunneling(Map * map, Coordinate source, DistanceMap * dist){
	return get_distance_map_hidden(map,source,dist,254);
}

int get_distance_map(Map * map, Coordinate source, DistanceMap * dist){
	return get_distance_map_hidden(map,source,dist,0);
}





