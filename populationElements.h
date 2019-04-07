#ifndef POPULATIONELEMENTS_H
#define POPULATIONELEMENTS_H
#include "mapElements.h"
#include <cstdint>
#include <string>
#include "dice.h"

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

class Entity{
protected:
	string *name;
	string *description;
	Dice attackDamage;
	int hitpoints;
	MonsterColor color;
public:
	Entity();
	Coordinate position;
	char symbol;
	int speed;
	bool dead;
	bool isPC;
};
class NPC : public Entity{
private:
	int rarity;
	//abilities bit vector
public:
	NPC();
	NPC(string *name, string *description,
		int speed, int hitpoints, Dice attackDamage,
		char symbol, MonsterColor color, int rarity /*, abilities*/);
	Coordinate (*move_strategy)(NPC * ent, DistanceMap * map, DistanceMap * mapTunnel);
};

void init_entity_npc(NPC *ent, Coordinate coord, char symbol, uint32_t characteristics);
void init_entity_pc(Entity *ent, Coordinate coord, char symbol);
void entity_get_move(NPC *ent, DistanceMap * map, DistanceMap * mapTunnel, Coordinate * coord);

#endif