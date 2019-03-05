#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include "populationElements.h"
#include "mapElements.h"

#define NPC_INTELLIGENT	0x0000000000000001
#define NPC_TELEPATHIC	0x0000000000000002
#define NPC_TUNNELING	0x0000000000000004
#define NPC_ERRATIC		0x0000000000000008


static void descend(Coordinate position, DistanceMap * dist, Coordinate * ret){
	int x,y;
	*ret = position;
	int minDist = INT_MAX;
	for(y=-1;y<=1;y++){
		for(x=-1;x<=1;x++){
			int curDist = get_distance(dist,position.x + x,position.y + y);
			if((curDist < minDist)
				||(curDist == minDist && rand()%2==0)){
				minDist = curDist;
				ret->x = position.x+x;
				ret->y = position.y+y;
			}
		}
	}
}

Coordinate tunnelMove(Entity * ent, DistanceMap * map, DistanceMap * mapTunnel){
	Coordinate ret;
	descend(ent->position,mapTunnel,&ret);
	return ret;
}

Coordinate nonTunnelMove(Entity * ent, DistanceMap * map, DistanceMap * mapTunnel){
	Coordinate ret;
	descend(ent->position,map,&ret);
	return ret;
}

Coordinate rightMove(Entity * ent, DistanceMap * map, DistanceMap * mapTunnel){
	Coordinate ret;
	ret.x = ent->position.x+1;
	ret.y = ent->position.y;
	return ret;
}

void init_entity_npc(Entity *ent, Coordinate coord, char symbol, uint32_t characteristics){
	ent->position = coord;
	ent->symbol = symbol;
	ent->dead = false;
	ent->speed = (rand()%10) + 5;
	ent->isPC = false;
	//end->pc = NULL;
	//ent->npc = &npc;
	if(characteristics & NPC_TUNNELING){
		ent->move_strategy = tunnelMove;
	}else{
		ent->move_strategy = nonTunnelMove;
	}
}

void init_entity_pc(Entity *ent, Coordinate coord, char symbol){
	ent->position = coord;
	ent->symbol = symbol;
	ent->dead = false;
	ent-> speed = 5;
	ent-> isPC = true;
	//end->pc = &pc;
	//ent->npc = NULL;
	ent->move_strategy = rightMove;
}

void entity_get_move(Entity *ent, DistanceMap * map, DistanceMap * mapTunnel, Coordinate * coord){
	*coord = ent->move_strategy(ent, map, mapTunnel);
}
