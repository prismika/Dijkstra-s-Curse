#ifndef ITEMPARSER_H
#define ITEMPARSER_H

#include <string>
#include <ostream>
#include <vector>
#include "dice.h"
#include "populationElements.h"

using namespace std;

class ItemBlueprint{
private:
	string name, description;
	ItemType type;
	MonsterColor color;
	Dice hit, damage, dodge, defence, weight, speed, attribute, value;
	bool artifact;
	int rarity;

  int howManyExist;
public:
	ItemBlueprint();
	ItemBlueprint(string &name,
           string &description,
           ItemType type,
           MonsterColor color,
           Dice &hit,
           Dice &damage,
           Dice &dodge,
           Dice &defence,
           Dice &weight,
           Dice &speed,
           Dice &attribute,
           Dice &value,
           bool artifact,
           int rarity);
	int getRarity();
	//Dynamically allocates new item
	Item * build();
  bool isBuildable();
	friend ostream & operator<<(ostream &out, const ItemBlueprint &r);
};

/*Beware! Malloc's the returned list and every item in it!
They must all be freed!*/
vector<ItemBlueprint> parser_load_item_list();

#endif