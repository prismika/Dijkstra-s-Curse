#include <limits.h>
#include <stdlib.h>
#include "populationElements.h"
#include "mapElements.h"
#include "dice.h"

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

Coordinate erraticMove(NPC * ent, DistanceMap * map, DistanceMap * mapTunnel){
	Coordinate ret = ent->position;
	ret.y += rand()%3 - 1;
	ret.x += rand()%3 - 1;
	return ret;
}

Coordinate tunnelMove(NPC * ent, DistanceMap * map, DistanceMap * mapTunnel){
	Coordinate ret;
	descend(ent->position,mapTunnel,&ret);
	return ret;
}

Coordinate tunnelMoveErratic(NPC * ent, DistanceMap * map, DistanceMap * mapTunnel){
	if(rand()%2){
		return erraticMove(ent,map,mapTunnel);
	}else{
		return tunnelMove(ent,map,mapTunnel);
	}
}

Coordinate nonTunnelMove(NPC * ent, DistanceMap * map, DistanceMap * mapTunnel){
	Coordinate ret;
	descend(ent->position,map,&ret);
	return ret;
}

Coordinate nonTunnelMoveErratic(NPC * ent, DistanceMap * map, DistanceMap * mapTunnel){
	if(rand()%2){
		return erraticMove(ent,map,mapTunnel);
	}else{
		return nonTunnelMove(ent,map,mapTunnel);
	}
}

Coordinate rightMove(NPC * ent, DistanceMap * map, DistanceMap * mapTunnel){
	Coordinate ret;
	ret.x = ent->position.x+1;
	ret.y = ent->position.y;
	return ret;
}

void init_entity_npc(NPC *ent, Coordinate coord, char symbol, uint32_t characteristics){
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
}

void entity_get_move(NPC *ent, DistanceMap * map, DistanceMap * mapTunnel, Coordinate * coord){
	*coord = ent->move_strategy(ent, map, mapTunnel);
}

Entity::Entity():hitpoints(0),color(MONSTER_RED),symbol('m'),
speed(10), dead(0){
	this->name = new string();
	this->description = new string();
}

MonsterColor Entity::getColor(){
	return MONSTER_CYAN;
}

NPC::NPC():Entity(){
	this->isPC = false;
	this->move_strategy = nonTunnelMove;
}

NPC::NPC(string *name, string *description, vector<MonsterAbility> abilityList,
	int speed, int hitpoints, Dice attackDamage,
	char symbol, MonsterColor color, int rarity,
	Coordinate position){
	this->name = name;
	this->description = description;
	this->speed = speed;
	this->hitpoints = hitpoints;
	this->attackDamage = attackDamage;
	this->symbol = symbol;
	this->color = color;
	this->rarity = rarity;
	this->isPC = false;
	this->position = position;
	struct{
		bool SMART=false;
		bool TELE=false;
		bool TUNNEL=false;
		bool ERRATIC=false;
		bool PASS=false;
		bool PICKUP=false;
		bool DESTROY=false;
		bool UNIQ=false;
		bool BOSS=false;
	}flags;
	for(size_t i = 0; i < abilityList.size(); i++){
		switch(abilityList[i]){
		case MONSTER_ABILITY_SMART:
			flags.SMART=true;
			break;
		case MONSTER_ABILITY_TELE:
			flags.TELE=true;
			break;
		case MONSTER_ABILITY_TUNNEL:
			flags.TUNNEL=true;
			break;
		case MONSTER_ABILITY_ERRATIC:
			flags.ERRATIC=true;
			break;
		case MONSTER_ABILITY_PASS:
			flags.PASS=true;
			break;
		case MONSTER_ABILITY_PICKUP:
			flags.PICKUP=true;
			break;
		case MONSTER_ABILITY_DESTROY:
			flags.DESTROY=true;
			break;
		case MONSTER_ABILITY_UNIQ:
			flags.UNIQ=true;
			break;
		case MONSTER_ABILITY_BOSS:
			flags.BOSS=true;
			break;
			default:
			break;
		}
	}
	if(flags.TUNNEL && flags.ERRATIC){
		this->move_strategy = tunnelMoveErratic;
	}else if(!flags.TUNNEL && flags.ERRATIC){
		this->move_strategy = nonTunnelMoveErratic;
	}else if(flags.TUNNEL){
		this->move_strategy = tunnelMove;
	}else{
		this->move_strategy = nonTunnelMove;
	}
}

int NPC::getRarity(){
	return this->rarity;
}
