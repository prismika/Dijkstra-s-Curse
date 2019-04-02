#ifndef MONSTERPARSER_H
#define MONSTERPARSER_H

#include <string>
#include <ostream>

typedef enum{
	RED, GREEN, BLUE, CYAN, YELLOW, MAGENTA, WHITE, BLACK
}MonsterColor;

class MonsterBlueprint{
private:
	std::string name;
	std::string description;
	MonsterColor color;
	int speed;
	std::string * abilities;
	int abilitiesSize;
	int hitpoints;
	int attackDamage;
	char symbol;
	int rarity;
public:
	friend std::ostream & operator<<(std::ostream &out, const MonsterBlueprint &r){
		return out << "This is a monster. What did you expect?" << std::endl;
	}
};

MonsterBlueprint * parser_load_monster_list(int * listSize);

#endif