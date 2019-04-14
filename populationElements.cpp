#include <limits.h>
#include <stdlib.h>
#include "populationElements.h"
#include "mapElements.h"
#include "dice.h"


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

void entity_get_move(NPC *ent, DistanceMap * map, DistanceMap * mapTunnel, Coordinate * coord){
	*coord = ent->move_strategy(ent, map, mapTunnel);
}

Entity::Entity():hitpoints(0),speed(0),symbol('m'),dead(0){
	
}

MonsterColor Entity::getColor(){
	return this->color;
}

int Entity::getSpeed(){
	return this->speed;
}

int Entity::rollDamage(){
	return this->attackDamage.roll();
}

PC::PC(){
	this->symbol = '@';
	this->speed = 10;
	this->hitpoints = 1000;
	this->name = new string("The Hero");
	this->description = new string("A fearless hero that refuses\nto accept the Curse of Dijkstra");
	this->isPC = true;
	this->color = MONSTER_CYAN;
	this->attackDamage.set(0,1,4);
	this->dead = false;
	int i;
	for(i = 0; i < INVENTORY_SIZE; i++){
		this->inventory[i] = NULL;
	}
	this->itemsInInventory = 0;
	for(i = 0; i < EQUIPMENT_SIZE; i++){
		this->equipment[i] = NULL;
	}
}

int PC::getSpeed(){
	int total = this->speed;
	int slot;
	for(slot = 0; slot < EQUIPMENT_SIZE; slot++){
		if(this->hasEquipmentItem(slot)){
			total += getEquipmentItem(slot)->getSpeed();
		}
	}
	return total;
}

int PC::rollDamage(){
	int total = this->attackDamage.roll();
	int slot;
	for(slot = 0; slot < EQUIPMENT_SIZE; slot++){
		if(this->hasEquipmentItem(slot)){
			total+= getEquipmentItem(slot)->rollDamage();
		}
	}
	return total;
}

int PC::setPosition(Coordinate coord){
	this->position = coord;
	return 0;
}

bool PC::hasInventoryItem(int slot){
	return this->inventory[slot] != NULL;
}

bool PC::hasEquipmentItem(int slot){//STUB
	return this->equipment[slot] != NULL;
}

Item * PC::getInventoryItem(int slot){
	return this->inventory[slot];
}

Item * PC::getEquipmentItem(int slot){
	return this->equipment[slot];
}

int PC::giveItem(Item * item){
	int i;
	for(i = 0; i < INVENTORY_SIZE; i++){
		if(!(this->inventory[i])){
			this->inventory[i] = item;
			this->itemsInInventory++;
			return i;
		}
	}
	return -1;
}

Item * PC::dropSlot(int slot){
	Item * toDrop = this->inventory[slot];
	this->inventory[slot] = NULL;
	return toDrop;
}

int PC::equip(int inventorySlot){//STUB
	if(!this->inventory[inventorySlot]){
		return -1;
	}
	Item * toEquip = this->inventory[inventorySlot];
	int equipmentSlot = -1;
	switch(toEquip->getType()){
		case ITEM_TYPE_WEAPON:
		equipmentSlot = 0;
		break;
		case ITEM_TYPE_OFFHAND:
		equipmentSlot = 1;
		break;
		case ITEM_TYPE_RANGED:
		equipmentSlot = 2;
		break;
		case ITEM_TYPE_ARMOR:
		equipmentSlot = 3;
		break;
		case ITEM_TYPE_HELMET:
		equipmentSlot = 4;
		break;
		case ITEM_TYPE_CLOAK:
		equipmentSlot = 5;
		break;
		case ITEM_TYPE_GLOVES:
		equipmentSlot = 6;
		break;
		case ITEM_TYPE_BOOTS:
		equipmentSlot = 7;
		break;
		case ITEM_TYPE_AMULET:
		equipmentSlot = 8;
		break;
		case ITEM_TYPE_LIGHT:
		equipmentSlot = 9;
		break;
		case ITEM_TYPE_RING:
		if(equipment[10] == NULL){
			equipmentSlot = 10;
		}else{
			equipmentSlot = 11;
		}
		break;
		default:
		return -2;
	}
	//The slots swap places, even if equipment slot is NULL
	this->inventory[inventorySlot] = this->equipment[equipmentSlot];
	this->equipment[equipmentSlot] = toEquip;
	return 0;
}

int PC::unequip(int equipmentSlot){
	if(equipment[equipmentSlot] == NULL){
		return -1;
	}
	int i;
	for(i = 0; i < INVENTORY_SIZE; i++){
		if(!(this->inventory[i])){
			this->inventory[i] = this->equipment[equipmentSlot];
			this->equipment[equipmentSlot] = NULL;
			return i;
		}
	}
	return -2;
}

Item * PC::destroy(int inventorySlot){
	Item * item = this->inventory[inventorySlot];
	this->inventory[inventorySlot] = NULL;
	return item;
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




//---------------------Items------------------------
Item::Item()
	:name("Default Item!"),
	description("I describe a default item!"),
	type(ITEM_TYPE_no_type),
	color(MONSTER_WHITE),
	hit(0),
	damage(Dice()),
	dodge(0),
	defence(0),
	weight(0),
	speed(0),
	attribute(0),
	value(0),
	artifact(0),
	rarity(0),
	symbol('*')
{

}

Item::Item(string name, string description,
	ItemType type, MonsterColor color,
	int hit, Dice damage, int dodge, int defence,
	int weight, int speed, int attribute, int value,
	bool artifact, int rarity)
	:name(name),
	description(description),
	type(type),
	color(color),
	hit(hit),
	damage(damage),
	dodge(dodge),
	defence(defence),
	weight(weight),
	speed(speed),
	attribute(attribute),
	value(value),
	artifact(artifact),
	rarity(rarity){
	char symbols[] = {'*','|',')','}','[',']','(','{','\\','=','"','_','~','?','!','$','/','\'','-','%'};
	this->symbol = symbols[(int)type];
}

char Item::getSymbol(){
	return this->symbol;
}

MonsterColor Item::getColor(){
	return this->color;
}

string *Item::getName(){
	return &this->name;
}

int Item::getSpeed(){
	return this->speed;
}

ItemType Item::getType(){
	return this->type;
}

int Item::rollDamage(){
	return this->damage.roll();
}
