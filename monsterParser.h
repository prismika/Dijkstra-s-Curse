#ifndef MONSTERPARSER_H
#define MONSTERPARSER_H

#include <string>
#include <ostream>

typedef enum{
	MONSTER_RED, MONSTER_GREEN, MONSTER_BLUE, MONSTER_CYAN, MONSTER_YELLOW, MONSTER_MAGENTA, MONSTER_WHITE, MONSTER_BLACK
}MonsterColor;

class Dice{
private:
	int base;
	int count;
	int sides;
public:
	Dice(void){}
	Dice(int base, int count, int sides);
	Dice(std::string);
	int roll();
};

class MonsterBlueprint{
private:
	char *name;
	char *description;
	Dice speed;
	Dice hitpoints;
	Dice attackDamage;
	char symbol;
	MonsterColor color;
	int rarity;
	char * *abilities;
	int abilitiesSize;
public:
	MonsterBlueprint(char * name, char * description,
		Dice speed, Dice hitpoints, Dice attackDamage,
		char symbol, MonsterColor color, int rarity);
	//Add ability
	friend std::ostream & operator<<(std::ostream &out, const MonsterBlueprint &r);
	~MonsterBlueprint(){
		//TODO free stuff here
	}
};

/*Beware! Malloc's the returned list and every item in it!
They must all be freed!*/
MonsterBlueprint * parser_load_monster_list(int * listSize);

#endif