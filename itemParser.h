#ifndef ITEMPARSER_H
#define ITEMPARSER_H

#include <string>
#include <ostream>
#include <vector>
#include "dice.h"
#include "populationElements.h"

using namespace std;

typedef enum
{
	ITEM_TYPE_no_type,
	ITEM_TYPE_WEAPON,
	ITEM_TYPE_OFFHAND,
	ITEM_TYPE_RANGED,
	ITEM_TYPE_ARMOR,
	ITEM_TYPE_HELMET,
	ITEM_TYPE_CLOAK,
	ITEM_TYPE_GLOVES,
	ITEM_TYPE_BOOTS,
	ITEM_TYPE_RING,
	ITEM_TYPE_AMULET,
	ITEM_TYPE_LIGHT,
	ITEM_TYPE_SCROLL,
	ITEM_TYPE_BOOK,
	ITEM_TYPE_FLASK,
	ITEM_TYPE_GOLD,
	ITEM_TYPE_AMMUNITION,
	ITEM_TYPE_FOOD,
	ITEM_TYPE_WAND,
	ITEM_TYPE_CONTAINER
}ItemType;

class ItemBlueprint{
private:
	string name, description;
	ItemType type;
	MonsterColor color;
	Dice hit, damage, dodge, defence, weight, speed, attribute, value;
	bool artifact;
	int rarity;
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
	friend ostream & operator<<(ostream &out, const ItemBlueprint &r);
};

/*Beware! Malloc's the returned list and every item in it!
They must all be freed!*/
vector<ItemBlueprint> parser_load_item_list();

#endif