#ifndef MONSTERPARSER_H
#define MONSTERPARSER_H

#include <string>
#include <ostream>
#include <vector>

using namespace std;

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
	Dice(string s);
	int roll();
	friend std::ostream & operator<<(std::ostream &out, const Dice &d);
};

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