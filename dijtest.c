#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "mapElements.h"
#include "dij.h"
#include "display.h"


int main(int argc, char *argv[]){
	Map map;
	map_init(&map);
	Coordinate source;
	source.x = 16;
	source.y = 17;
	DistanceMap dist;
	get_distance_map(&map,source,&dist);

	display_distance_map(&dist);

	return 0;
}