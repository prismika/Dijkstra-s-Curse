#include <stdlib.h>
#include "mapElements.h"
#include "map.h"
#include "mapPopulator.h"
#include "pathFinder.h"

#define PC_BUBBLE_SIZE 8

Coordinate nextCoord(Coordinate coord){
	if(coord.x<MAPWIDTH - 1){
		coord.x += 1;
		return coord;
	}
	if(coord.y<MAPHEIGHT - 1){
		coord.x = 0;
		coord.y += 1;
		return coord;
	}
	coord.x = 0;
	coord.y = 0;
	return coord;
}
Coordinate findOpenBlock(Map * map, Coordinate pcCoord){
	BlockType canPlaceOn[] = {floor};
	int canPlaceOnSize = 1;
	Coordinate startingCoord;
	Coordinate curCoord;
	map_choose_random_block(map,canPlaceOn,canPlaceOnSize,&startingCoord);
	curCoord = nextCoord(startingCoord);
	Block curBlock;
	//Until we get back to where we started
	while(!(curCoord.x==startingCoord.x && curCoord.y==startingCoord.y)){
		map_getBlock(map, curCoord.x, curCoord.y, &curBlock);
		if(distLInf(curCoord, pcCoord) < PC_BUBBLE_SIZE
			||map_has_entity_at(map, curCoord.x, curCoord.y)
			||curBlock.hardness>0){
			curCoord = nextCoord(curCoord);
			continue;
		}
		return curCoord;
	}
	curCoord.x = curCoord.y = -1;
	return curCoord;

}
int populate_map(Map * map, int nummon){
	Coordinate pcCoord;
	BlockType canPlaceOn[] = {floor};
	int canPlaceOnSize = 1;
	map_choose_random_block(map,canPlaceOn,canPlaceOnSize,&pcCoord);
	map_new_pc(map,pcCoord);
	DistanceMap * distNonTunnel = map_get_distance_map_non_tunneling(map);
	DistanceMap * distTunnel = map_get_distance_map_tunneling(map);
	get_distance_map(map, pcCoord, distNonTunnel);
	get_distance_map_tunneling(map, pcCoord, distTunnel);

	int i;
	for(i=0; i<nummon;i++){
		Coordinate curCoord = findOpenBlock(map, pcCoord);
		if(curCoord.x == -1){
			return 0;
		}
		int chars = rand()%16;
		map_new_npc(map,curCoord,chars);
	}
	return 0;
}
