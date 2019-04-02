#include "monsterParser.h"
#include <cstdlib>



MonsterBlueprint * parser_load_monster_list(int * listSize){
	MonsterBlueprint * blueprintList = (MonsterBlueprint*) malloc(sizeof(MonsterBlueprint*));
	*listSize = 1;
	Dice d1,d2,d3;
	MonsterBlueprint blueprint((char*)"Gregory",(char*)"He's a fast one",d1,d2,d3,
		'G',MONSTER_RED,10);
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
	char * colorName;
	switch(r.color){
		case MONSTER_RED:
		colorName = (char*) "Red";
		break;
		case MONSTER_GREEN:
		colorName = (char*) "Green";
		break;
		case MONSTER_BLUE:
		colorName = (char*) "Blue";
		break;
		case MONSTER_CYAN:
		colorName = (char*) "Cyan";
		break;
		case MONSTER_YELLOW:
		colorName = (char*) "Yellow";
		break;
		case MONSTER_MAGENTA:
		colorName = (char*) "Magenta";
		break;
		case MONSTER_WHITE:
		colorName = (char*) "White";
		break;
		case MONSTER_BLACK:
		colorName = (char*) "Black";
		break;
	}
	out << "Name: " << r.name << std::endl;
	out << "Symbol: " << r.symbol << std::endl;
	out << "Color: " << colorName << std::endl;
	out << "Rarity: " << r.rarity << std::endl;
	//out << "Speed: " << r.speed << std::endl;
	//out << "Hitpoints: " <<r.hitpoints << std::endl;
	//out << "Attack: " <<r.attackDamage << std::endl;
	out << "Abilities: ";
	int i;
	for(i=0; i<r.abilitiesSize; i++){
		out << r.abilities[i] << " ";
	}
	out<<std::endl;

	out << r.description << std::endl;

	return out;
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
