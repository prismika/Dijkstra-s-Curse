#include "monsterParser.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

using namespace std;


std::vector<MonsterBlueprint> parser_load_monster_list(int * listSize){

	ifstream file;
	std::vector <MonsterBlueprint> blueprintList;
	int l = strlen(getenv("HOME")) + strlen("/.rlg327/monster_desc.txt") + 1; 
	char * filePath = (char *) malloc(l);
	strcpy(filePath,getenv("HOME"));
	strcat(filePath,"/.rlg327/monster_desc.txt");
	file.open(filePath);

	string curLine;
	Dice d1,d2,d3;
	char *name, *desc, *abilities;
	char symbol = '0';
	MonsterColor color = MONSTER_RED;
	int rarity = 10;
	while(getline(file, curLine)&&curLine.compare("BEGIN MONSTER")){
	}
	//Inside a monster definition
	while(getline(file, curLine) && curLine.compare("END")){
		cout << "Parsing: '" << curLine << "'" << endl;
		string keyword = curLine.substr(0, curLine.find(" "));
		curLine.erase(0,curLine.find(" ")+1);
		cout << "Keyword is '" << keyword << "'" << endl;
		cout << "What's left is '" << curLine << "'" <<endl;
		if(!keyword.compare("NAME")){
			name = strdup(curLine.c_str());
		}
	}
	//Reached the end

	MonsterBlueprint *blueprint = new MonsterBlueprint(
		name,desc,abilities,
		d1,d2,d3,
		symbol,color,rarity);
	blueprintList.push_back(*blueprint);

	*listSize = blueprintList.size();
	return blueprintList;
}

MonsterBlueprint::MonsterBlueprint(char * name, char * description, char* abilities,
	Dice speed, Dice hitpoints, Dice attackDamage,
	char symbol, MonsterColor color, int rarity){
	this->name = name;
	this->description = description;
	this->abilities = abilities;
	this->speed = speed;
	this->hitpoints = hitpoints;
	this->attackDamage = attackDamage;
	this->symbol = symbol;
	this->color = color;
	this->rarity = rarity;
}

// int MonsterBlueprint::addAbility(char * newAbility){
// 	char * newAbilityCopy = strdup(newAbility);
// 	abilities.push_back(newAbilityCopy);
// 	return 0;
// }

MonsterBlueprint & MonsterBlueprint::operator=(const MonsterBlueprint & r){
	this->name = r.name;
	this->symbol = r.symbol;
	this->speed = r.speed;
	this->color = r.color;
	this->hitpoints = r.hitpoints;
	this->attackDamage = r.attackDamage;
	this->symbol = r.symbol;
	this->color = r.color;
	this->rarity = r.rarity;
	this->description = r.description;
	this->abilities = r.abilities;

	return *this;
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
	out << "Speed: " << r.speed << std::endl;
	out << "Hitpoints: " << r.hitpoints << std::endl;
	out << "Attack: " << r.attackDamage << std::endl;
	out << "Abilities: " <<r.abilities <<std::endl;
	out << r.description << std::endl;

	return out;
}

Dice::Dice(void){
	this->base = 1;
	this->count = 2;
	this->sides = 3;
}

Dice::Dice(int base, int count, int sides){
	this->base = base;
	this->count = count;
	this->sides = sides;
}


int Dice::roll(){
	int sum = base;
	for(int i = 0; i < count; i++){
		sum += rand()%sides + 1;
	}
	return sum;
}

std::ostream & operator<<(std::ostream &out, const Dice &d){
	return out << d.base << " + " << d.count << "d" << d.sides;
}
