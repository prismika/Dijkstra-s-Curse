#include "monsterParser.h"
#include <cstdlib>



MonsterBlueprint * parser_load_monster_list(int * listSize){
	MonsterBlueprint * blueprintList = (MonsterBlueprint*) malloc(sizeof(MonsterBlueprint*));
	*listSize = 1;
	Dice d1,d2,d3;
	MonsterBlueprint blueprint((char*)"Gregory",(char*)"He's a fast one",d1,d2,d3,
		'x',MONSTER_RED,10);
	blueprintList[0] = blueprint;
	return blueprintList;
}

MonsterBlueprint::MonsterBlueprint(char * name, char * description,
	Dice speed, Dice hitpoints, Dice attackDamage,
	char symbol, MonsterColor color, int rarity){
	this->name = name;
	this->description = description;
	this->speed = speed;
	this->hitpoints = hitpoints;
	this->attackDamage = attackDamage;
	this->symbol = symbol;
	this->color = color;
	this->rarity = rarity;

	this->abilities = (char * *) malloc(0);
	this->abilitiesSize = 0;
}

std::ostream & operator<<(std::ostream &out, const MonsterBlueprint &r){
	return out << "Name: "
	<< r.name <<std::endl;
}

Dice::Dice(int base, int count, int sides){
	this->base = base;
	this->count = count;
	this->sides = sides;
}

Dice::Dice(std::string){
	this->base = 2;
	this->count = 2;
	this->sides = 3;
}

int Dice::roll(){
	int sum = base;
	for(int i = 0; i < count; i++){
		sum += rand()%sides + 1;
	}
	return sum;
}
