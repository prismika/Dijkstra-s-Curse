#include "populationElements.h"
#include "mapElements.h"

void init_entity_npc(Entity *ent, Coordinate coord, char symbol, uint32_t characteristics){
	ent->position = coord;
	ent->symbol = symbol;

	//ent->npc = &npc
	switch(characteristics){
		//Assign movement strategy to npc
	}
}
