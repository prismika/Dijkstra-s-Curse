#ifndef MONSTERPARSER_H
#define MONSTERPARSER_H

#include <string>
#include <ostream>
#include <vector>
#include "dice.h"
#include "populationElements.h"

using namespace std;


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
	string abilities;
public:
	MonsterBlueprint(string name, string description, string abilities,
		Dice speed, Dice hitpoints, Dice attackDamage,
		char symbol, MonsterColor color, int rarity);
	friend std::ostream & operator<<(std::ostream &out, const MonsterBlueprint &r);
	~MonsterBlueprint(){
		//TODO free stuff here
	}
	MonsterBlueprint & operator=(const MonsterBlueprint &r);
};

/*Beware! Malloc's the returned list and every item in it!
They must all be freed!*/
std::vector<MonsterBlueprint> parser_load_monster_list(int * listSize);

#endif