#ifndef MAPPOPULATOR_H
#define MAPPOPULATOR_H

#include <vector>
#include "map.h"
#include "mapElements.h"
#include "monsterParser.h"
#include "itemParser.h"

class MapPopulator{
private:
	vector<MonsterBlueprint> monsterAtlas;
	vector<ItemBlueprint> itemAtlas;
	void populateWithItems(Map * map, int numitems);
public:
	MapPopulator();
	int populate(Map * map, PC * pc, int nummon);
};


#endif