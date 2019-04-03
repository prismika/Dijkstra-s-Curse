#include "monsterParser.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

using namespace std;


vector<MonsterBlueprint> parser_load_monster_list(int * listSize){

	ifstream file;
	vector <MonsterBlueprint> blueprintList;
	int l = strlen(getenv("HOME")) + strlen("/.rlg327/monster_desc.txt") + 1; 
	char * filePath = (char *) malloc(l);
	strcpy(filePath,getenv("HOME"));
	strcat(filePath,"/.rlg327/monster_desc.txt");
	file.open(filePath);

	string curLine;
	Dice HPd,ATTd,SPd;
	string name, desc, abilities;
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
			name = curLine;
		}else if(!keyword.compare("DESC")){
			while(getline(file,curLine) && curLine.compare(".")){
				desc.append(curLine);
				desc.append("\n");
			}
		}else if(!keyword.compare("ABIL")){
			abilities = curLine;
		}else if(!keyword.compare("SYMB")){
			symbol = curLine[0];
		}else if(!keyword.compare("RRTY")){
			rarity = atoi(curLine.c_str());
		}else if(!keyword.compare("COLOR")){
			curLine = curLine.substr(0, curLine.find(" "));
			if(!curLine.compare("RED")){
				color = MONSTER_RED;
			}else if(!curLine.compare("GREEN")){
				color = MONSTER_GREEN;
			}else if(!curLine.compare("BLUE")){
				color = MONSTER_BLUE;
			}else if(!curLine.compare("CYAN")){
				color = MONSTER_CYAN;
			}else if(!curLine.compare("YELLOW")){
				color = MONSTER_YELLOW;
			}else if(!curLine.compare("MAGENTA")){
				color = MONSTER_MAGENTA;
			}else if(!curLine.compare("WHITE")){
				color = MONSTER_WHITE;
			}else if(!curLine.compare("BLACK")){
				color = MONSTER_BLACK;
			}
		}else if(!keyword.compare("HP")){
			Dice newDice(curLine);
			HPd = newDice;
		}else if(!keyword.compare("DAM")){
			Dice newDice(curLine);
			ATTd = newDice;
		}else if(!keyword.compare("SPEED")){
			Dice newDice(curLine);
			SPd = newDice;
		}
	}
	cout << endl << endl;
	//Reached the end

	MonsterBlueprint *blueprint = new MonsterBlueprint(
		name,desc,abilities,
		SPd,HPd,ATTd,
		symbol,color,rarity);
	blueprintList.push_back(*blueprint);

	*listSize = blueprintList.size();
	return blueprintList;
}

MonsterBlueprint::MonsterBlueprint(string name, string description, string abilities,
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

ostream & operator<<(ostream &out, const MonsterBlueprint &r){
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
	out << "Name: " << r.name << endl;
	out << endl << "Description: " << r.description << endl;
	out << endl << "Color: " << colorName << endl;
	out << endl << "Speed: " << r.speed << endl;
	out << endl << "Abilities: " <<r.abilities <<endl;
	out << endl << "Hitpoints: " << r.hitpoints << endl;
	out << endl << "Attack: " << r.attackDamage << endl;
	out << endl << "Symbol: " << r.symbol << endl;
	out << endl << "Rarity: " << r.rarity << endl;
	out << endl;

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

Dice::Dice(string s){
	base = stoi(s.substr(0, s.find("+")));
	s.erase(0,s.find("+")+1);
	count = stoi(s.substr(0, s.find("d")));
	s.erase(0,s.find("d")+1);
	sides = stoi(s);
	cout<<"Created dice: " << *this <<endl;
}

int Dice::roll(){
	int sum = base;
	for(int i = 0; i < count; i++){
		sum += rand()%sides + 1;
	}
	return sum;
}

ostream & operator<<(ostream &out, const Dice &d){
	return out << d.base << " + " << d.count << "d" << d.sides;
}
