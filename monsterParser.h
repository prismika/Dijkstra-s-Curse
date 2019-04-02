#ifndef MONSTERPARSER_H
#define MONSTERPARSER_H

#include <string>
#include <ostream>
#include <vector>

typedef enum{
	MONSTER_RED,
	MONSTER_GREEN,
	MONSTER_BLUE,
	MONSTER_CYAN,
	MONSTER_YELLOW,
	MONSTER_MAGENTA,
	MONSTER_WHITE,
	MONSTER_BLACK
}MonsterColor;

class Dice{
private:
	int base;
	int count;
	int sides;
public:
	Dice(void);
	Dice(int base, int count, int sides);
	int roll();
	friend std::ostream & operator<<(std::ostream &out, const Dice &d);
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
	char *abilities;
public:
	MonsterBlueprint(char * name, char * description, char* abilities,
		Dice speed, Dice hitpoints, Dice attackDamage,
		char symbol, MonsterColor color, int rarity);
	int addAbility(char * newAblility);
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