#include <stdlib.h>
#include "mapElements.h"
#include "map.h"
#include "mapPopulator.h"
#include "pathFinder.h"
#include "monsterParser.h"
#include "itemParser.h"

#define PC_BUBBLE_SIZE 8

using namespace std;

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
static Coordinate findOpenBlock(Map * map, Coordinate pcCoord){
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

MapPopulator::MapPopulator(){
	monsterAtlas = parser_load_monster_list();
	itemAtlas = parser_load_item_list();
}

int MapPopulator::populate(Map * map, PC * pc, int nummon){
	populateWithItems(map, 20);
	Entity * entityList[MAPHEIGHT*MAPWIDTH];
	int entityListIndex = 0;
	Coordinate pcCoord;
	BlockType canPlaceOn[] = {floor};
	int canPlaceOnSize = 1;
	map_choose_random_block(map,canPlaceOn,canPlaceOnSize,&pcCoord);
	pc->setPosition(pcCoord);
	map_set_entity(map,pcCoord.x,pcCoord.y,pc);
	entityList[entityListIndex++] = pc;
	DistanceMap * distNonTunnel = map_get_distance_map_non_tunneling(map);
	DistanceMap * distTunnel = map_get_distance_map_tunneling(map);
	get_distance_map(map, pcCoord, distNonTunnel);
	get_distance_map_tunneling(map, pcCoord, distTunnel);

	//Place monsters
	int i;
	for(i=0; i<nummon;i++){
		int monsterIndex;
		do{
			monsterIndex = rand()%monsterAtlas.size();
		}while(rand()%100 >= monsterAtlas[monsterIndex].getRarity());
		MonsterBlueprint blueprint = monsterAtlas[monsterIndex];
		Coordinate curCoord = findOpenBlock(map, pcCoord);
		if(curCoord.x == -1) break; //Map is full
		NPC *npc = blueprint.build(curCoord);
		map_set_entity(map, curCoord.x, curCoord.y, npc);
		entityList[entityListIndex++] = npc;
	}

	//Create tighter entity list
	Entity ** entityList_tight = (Entity **) malloc(sizeof(Entity *) * entityListIndex);
	for(i=0;i<entityListIndex; i++){
		entityList_tight[i] = entityList[i];
	}
	map->populationList = entityList_tight;
	map->populationListSize = entityListIndex;

	return 0;
}

void MapPopulator::populateWithItems(Map * map, int numitems){
	BlockType canPlaceOn[] = {floor};
	int canPlaceOnSize = 1;
	int i;
	for(i = 0; i < numitems; i++){
		Coordinate itemCoord;
		map_choose_random_block(map,canPlaceOn,canPlaceOnSize,&itemCoord);
		int itemIndex;
		do{
			itemIndex = rand()%itemAtlas.size();
		}while((rand()%100 >= itemAtlas[itemIndex].getRarity())
			|| (!itemAtlas[itemIndex].isBuildable()));
		Item * item = itemAtlas[itemIndex].build();
		map->placeItem(item,itemCoord);
	}
}
