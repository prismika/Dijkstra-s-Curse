#ifndef POPULATIONELEMENTS_H
#define POPULATIONELEMENTS_H
#include "mapElements.h"
#include <cstdint>
#include <string>
#include <vector>
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

typedef enum {
	MONSTER_ABILITY_SMART,
	MONSTER_ABILITY_TELE,
	MONSTER_ABILITY_TUNNEL,
	MONSTER_ABILITY_ERRATIC,
	MONSTER_ABILITY_PASS,
	MONSTER_ABILITY_PICKUP,
	MONSTER_ABILITY_DESTROY,
	MONSTER_ABILITY_UNIQ,
	MONSTER_ABILITY_BOSS
}MonsterAbility;

class Entity{
protected:
	string *name;
	string *description;
	Dice attackDamage;
	int hitpoints;
	MonsterColor color;
public:
	Entity();
	MonsterColor getColor();
	Coordinate position;
	char symbol;
	int speed;
	bool dead;
	bool isPC;
};
class NPC : public Entity{
private:
	int rarity;
public:
	NPC();
	NPC(string *name, string *description, vector<MonsterAbility> abilityList,
		int speed, int hitpoints, Dice attackDamage,
		char symbol, MonsterColor color, int rarity, Coordinate position);
	Coordinate (*move_strategy)(NPC * ent, DistanceMap * map, DistanceMap * mapTunnel);
	int getRarity();
};

void init_entity_npc(NPC *ent, Coordinate coord, char symbol, uint32_t characteristics);
void init_entity_pc(Entity *ent, Coordinate coord, char symbol);
void entity_get_move(NPC *ent, DistanceMap * map, DistanceMap * mapTunnel, Coordinate * coord);

#endif