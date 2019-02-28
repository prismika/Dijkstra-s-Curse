#include <stdlib.h>

#include "mapElements.h"
#include "map.h"
#include "mapPopulator.h"


int populate_map(Map * map, int nummon){

	int i;
	for(i=0; i<nummon;i++){
		Coordinate curCoord;
		BlockType canPlaceOn[] = {floor};
		int canPlaceOnSize = 1;
		map_choose_random_block(map,canPlaceOn,canPlaceOnSize,&curCoord);
		map_new_npc(map,curCoord,0);
	}
	return 0;
}