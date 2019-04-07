#ifndef MONSTERPARSER_H
#define MONSTERPARSER_H

#include <string>
#include <ostream>
#include <vector>
#include "dice.h"
#include "populationElements.h"
#include "mapElements.h"

using namespace std;

typedef enum {
	SMART,
	TELE,
	TUNNEL,
	ERRATIC,
	PASS,
	PICKUP,
	DESTROY,
	UNIQ,
	BOSS
}MonsterAbility;

class MonsterBlueprint{
private:
	string name;
	string description;
	Dice speed;
	Dice hitpoints;
	Dice attackDamage;
	char symbol;
	MonsterColor color;
	int rarity;
	vector<MonsterAbility> abilityList;
public:
	MonsterBlueprint(string name, string description,
		Dice speed, Dice hitpoints, Dice attackDamage,
		char symbol, MonsterColor color, int rarity);
	~MonsterBlueprint(){
		//TODO free stuff here
	}
	NPC * build(Coordinate position);
	friend std::ostream & operator<<(ostream &out, const MonsterBlueprint &r);
	MonsterBlueprint & operator=(const MonsterBlueprint &r);
};

/*Beware! Malloc's the returned list and every item in it!
They must all be freed!*/
vector<MonsterBlueprint> parser_load_monster_list();

#endif