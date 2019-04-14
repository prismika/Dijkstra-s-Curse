#ifndef POPULATIONELEMENTS_H
#define POPULATIONELEMENTS_H
#include "mapElements.h"
#include <cstdint>
#include <string>
#include <vector>
#include "dice.h"
using namespace std;

#define INVENTORY_SIZE 10
#define EQUIPMENT_SIZE 12

//---------------------Items-------------------------
//If you reorder this enum, God himself will smite you where you stand.
//Also display has a convenient lookup table that depends on this order.
typedef enum{
	ITEM_TYPE_no_type,
	ITEM_TYPE_WEAPON,
	ITEM_TYPE_OFFHAND,
	ITEM_TYPE_RANGED,
	ITEM_TYPE_ARMOR,
	ITEM_TYPE_HELMET,
	ITEM_TYPE_CLOAK,
	ITEM_TYPE_GLOVES,
	ITEM_TYPE_BOOTS,
	ITEM_TYPE_RING,
	ITEM_TYPE_AMULET,
	ITEM_TYPE_LIGHT,
	ITEM_TYPE_SCROLL,
	ITEM_TYPE_BOOK,
	ITEM_TYPE_FLASK,
	ITEM_TYPE_GOLD,
	ITEM_TYPE_AMMUNITION,
	ITEM_TYPE_FOOD,
	ITEM_TYPE_WAND,
	ITEM_TYPE_CONTAINER
}ItemType;

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

class Item{
private:
	string name;
	string description;
	ItemType type;
	MonsterColor color;
	int hit;
	Dice damage;
	int dodge, defence, weight, speed, attribute, value;
	bool artifact;
	int rarity;
	char symbol;
	int * howManyPickedUp;
	int * howManyExist;
public:
	Item();
	Item(string name, string description,
	ItemType type, MonsterColor color,
	int hit, Dice damage, int dodge, int defence,
	int weight, int speed, int attribute, int value,
	bool artifact, int rarity);
	void setHowManyPickedUp(int * p);
	void setHowManyExist(int * p);
	char getSymbol();
	string * getName();
	MonsterColor getColor();
	int getSpeed();
	ItemType getType();
	int rollDamage();
	string *getDescription();
	void pickedUp();
};

//--------------------Entities (living things)-------------------
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
	int speed;
public:
	Coordinate position;
	char symbol;
	bool dead;
	bool isPC;
	bool canTunnel;
	Entity();
	string * getName();
	string * getDescription();
	MonsterColor getColor();
	int getSpeed();
	int rollDamage();
	int attack(Entity * victim);
	bool takeDamage(int hp);
};

class PC : public Entity{
private:
	Item *inventory[INVENTORY_SIZE];
	Item *equipment[EQUIPMENT_SIZE];/*WEAPON, OFFHAND,
	RANGED, ARMOR, HELMET, CLOAK, GLOVES, BOOTS,
	AMULET, LIGHT, and two for RING*/
	int itemsInInventory;//Vestigial. Don't trust his lies.
public:
	PC();
	int getSpeed();
	int rollDamage();
	int setPosition(Coordinate position);
	bool hasInventoryItem(int slot);
	bool hasEquipmentItem(int slot);
	Item * getInventoryItem(int slot);
	Item * getEquipmentItem(int slot);
	/* Returns slot number. -1 if no open slot.*/
	int giveItem(Item * item);
	Item * dropSlot(int slot);
	int equip(int slot);
	/*Returns slot number. -1 if no such equipment, -2 if
	no opens slots in inventory*/
	int unequip(int slot);
	Item * destroy(int inventorySlot);
};

class NPC : public Entity{
private:
	int rarity;
	bool boss;
public:
	NPC();
	NPC(string *name, string *description, vector<MonsterAbility> abilityList,
		int speed, int hitpoints, Dice attackDamage,
		char symbol, MonsterColor color, int rarity, Coordinate position);
	Coordinate (*move_strategy)(NPC * ent, DistanceMap * map, DistanceMap * mapTunnel);
	int getRarity();
	bool isBoss();
};

void entity_get_move(NPC *ent, DistanceMap * map, DistanceMap * mapTunnel, Coordinate * coord);

#endif