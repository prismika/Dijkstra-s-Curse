#ifndef POPULATIONELEMENTS_H
#define POPULATIONELEMENTS_H
#include "mapElements.h"
#include <stdint.h>
#include <stdbool.h>

class Entity{
public:
	Coordinate position;
	char symbol;
	int speed;
	bool dead;
	bool isPC;
};
class NPC : public Entity{
private:

public:
	Coordinate (*move_strategy)(NPC * ent, DistanceMap * map, DistanceMap * mapTunnel);
};

void init_entity_npc(NPC *ent, Coordinate coord, char symbol, uint32_t characteristics);
void init_entity_pc(Entity *ent, Coordinate coord, char symbol);
void entity_get_move(NPC *ent, DistanceMap * map, DistanceMap * mapTunnel, Coordinate * coord);

#endif