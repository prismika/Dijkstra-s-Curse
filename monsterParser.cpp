#include "monsterParser.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

using namespace std;
#define VERSION_NAME "RLG327 MONSTER DESCRIPTION 1"



vector<MonsterBlueprint> parser_load_monster_list(int * listSize){
	ifstream file;
	vector <MonsterBlueprint> blueprintList;
	int l = strlen(getenv("HOME")) + strlen("/.rlg327/monster_desc.txt") + 1; 
	char * filePath = (char *) malloc(l);
	strcpy(filePath,getenv("HOME"));
	strcat(filePath,"/.rlg327/monster_desc.txt");
	file.open(filePath);

	string curLine;
	getline(file, curLine);
	if(curLine.compare(VERSION_NAME)){
		cerr << "Ope. The file did not begin with the correct version information." <<endl;
		cerr << curLine << endl;
		cerr << "Length " << curLine.length() << endl;
		cerr << VERSION_NAME << endl;
		cerr << (int)curLine[28] <<endl;
		cerr << "Length " << strlen(VERSION_NAME) << endl;
		exit(1);
	}
	while(true){
		nextMonsterDescription:
		class SectionsSeen{
		public:
			bool NAME = false;
			bool DESC = false;
			bool COLOR = false;
			bool SPEED = false;
			bool ABIL = false;
			bool HP = false;
			bool DAM = false;
			bool SYMB = false;
			bool RRTY = false;
		};
		SectionsSeen sectionsSeen;
		Dice HPd,ATTd,SPd;
		string name, desc, abilities;
		char symbol = '0';
		MonsterColor color = MONSTER_RED;
		int rarity = 10;
		while(getline(file, curLine)&&curLine.compare("BEGIN MONSTER")){
		}
		if(file.peek() == -1){
			goto fileFinished;
		}	
		//Inside a monster definition
		while(getline(file, curLine) && curLine.compare("END")){
			// cout << "Parsing: '" << curLine << "'" << endl;
			string keyword = curLine.substr(0, curLine.find(" "));
			curLine.erase(0,curLine.find(" ")+1);
			// cout << "Keyword is '" << keyword << "'" << endl;
			// cout << "What's left is '" << curLine << "'" <<endl;
			if(!keyword.compare("NAME")){
				if(sectionsSeen.NAME) goto nextMonsterDescription;
				sectionsSeen.NAME = true;
				name = curLine;
			}else if(!keyword.compare("DESC")){
				if(sectionsSeen.DESC) goto nextMonsterDescription;
				sectionsSeen.DESC = true;
				while(getline(file,curLine) && curLine.compare(".")){
					desc.append(curLine);
					desc.append("\n");
				}
			}else if(!keyword.compare("ABIL")){
				if(sectionsSeen.ABIL) goto nextMonsterDescription;
				sectionsSeen.ABIL = true;
				if(curLine.compare("ABIL")) abilities = curLine;
			}else if(!keyword.compare("SYMB")){
				if(sectionsSeen.SYMB) goto nextMonsterDescription;
				sectionsSeen.SYMB = true;
				symbol = curLine[0];
			}else if(!keyword.compare("RRTY")){
				if(sectionsSeen.RRTY) goto nextMonsterDescription;
				sectionsSeen.RRTY = true;
				rarity = atoi(curLine.c_str());
			}else if(!keyword.compare("COLOR")){
				if(sectionsSeen.COLOR) goto nextMonsterDescription;
				sectionsSeen.COLOR = true;
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
				if(sectionsSeen.HP) goto nextMonsterDescription;
				sectionsSeen.HP = true;
				Dice newDice(curLine);
				HPd = newDice;
			}else if(!keyword.compare("DAM")){
				if(sectionsSeen.DAM) goto nextMonsterDescription;
				sectionsSeen.DAM = true;
				Dice newDice(curLine);
				ATTd = newDice;
			}else if(!keyword.compare("SPEED")){
				if(sectionsSeen.SPEED) goto nextMonsterDescription;
				sectionsSeen.SPEED = true;
				Dice newDice(curLine);
				SPd = newDice;
			}
		}
		//Reached the end of this monster's description
		if(!sectionsSeen.NAME
			||!sectionsSeen.DESC
			||!sectionsSeen.COLOR
			||!sectionsSeen.SPEED
			||!sectionsSeen.ABIL
			||!sectionsSeen.HP
			||!sectionsSeen.DAM
			||!sectionsSeen.SYMB
			||!sectionsSeen.RRTY){
			goto nextMonsterDescription;
		}

		MonsterBlueprint *blueprint = new MonsterBlueprint(
			name,desc,abilities,
			SPd,HPd,ATTd,
			symbol,color,rarity);
		blueprintList.push_back(*blueprint);
	}
	fileFinished:
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
	out << r.name << endl;
	out << r.description;
	out << colorName << endl;
	out << r.speed << endl;
	out << r.abilities <<endl;
	out << r.hitpoints << endl;
	out << r.attackDamage << endl;
	out << r.symbol << endl;
	out << r.rarity << endl;
	out << endl;
	return out;
}
