#include "monsterParser.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

using namespace std;
#define VERSION_NAME "RLG327 MONSTER DESCRIPTION 1"

static void parse_abilities(string *s, vector<MonsterAbility> *abilityList){
	bool done = false;
	do{
		int spaceIndex = s->find(" ");
		string abilityString;
		if(spaceIndex == -1){
			abilityString = *s;
			done = true;
		}else{
			abilityString = s->substr(0, spaceIndex);
			s->erase(0,spaceIndex+1);
		}
		if(!abilityString.compare("SMART")){
			abilityList->push_back(MONSTER_ABILITY_SMART);
		}else if(!abilityString.compare("TELE")){
			abilityList->push_back(MONSTER_ABILITY_TELE);
		}else if(!abilityString.compare("TUNNEL")){
			abilityList->push_back(MONSTER_ABILITY_TUNNEL);
		}else if(!abilityString.compare("ERRATIC")){
			abilityList->push_back(MONSTER_ABILITY_ERRATIC);
		}else if(!abilityString.compare("PASS")){
			abilityList->push_back(MONSTER_ABILITY_PASS);
		}else if(!abilityString.compare("PICKUP")){
			abilityList->push_back(MONSTER_ABILITY_PICKUP);
		}else if(!abilityString.compare("DESTROY")){
			abilityList->push_back(MONSTER_ABILITY_DESTROY);
		}else if(!abilityString.compare("UNIQ")){
			abilityList->push_back(MONSTER_ABILITY_UNIQ);
		}else if(!abilityString.compare("BOSS")){
			abilityList->push_back(MONSTER_ABILITY_BOSS);
		}
	}while(!done);
}

vector<MonsterBlueprint> parser_load_monster_list(){
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
		string name, desc;
		vector<MonsterAbility> abilityList;
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
				if(curLine.compare("ABIL")) parse_abilities(&curLine,&abilityList);
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
			name,desc, abilityList,
			SPd,HPd,ATTd,
			symbol,color,rarity);
		blueprintList.push_back(*blueprint);
	}
	fileFinished:
	return blueprintList;
}

MonsterBlueprint::MonsterBlueprint(){}

MonsterBlueprint::MonsterBlueprint(string name, string description, vector<MonsterAbility> abilityList,
	Dice speed, Dice hitpoints, Dice attackDamage,
	char symbol, MonsterColor color, int rarity){
	this->name = new string(name);
	this->description = new string(description);
	this->abilityList = abilityList;
	this->speed = speed;
	this->hitpoints = hitpoints;
	this->attackDamage = attackDamage;
	this->symbol = symbol;
	this->color = color;
	this->rarity = rarity;
	this->unique = false;
	for(size_t i = 0; i < abilityList.size(); i++){
		if(abilityList[i] == MONSTER_ABILITY_UNIQ){
			this->unique = true;
		}
	}
	this->howManyExist = 0;
	this->howManyDead = 0;
}

int MonsterBlueprint::getRarity(){
	return this->rarity;
}

bool MonsterBlueprint::isBuildable(){
	return !unique
		|| ((howManyExist == 0) && (howManyDead == 0));
		// || (howManyExist == 0);
}

NPC * MonsterBlueprint::build(Coordinate position){
	this->howManyExist++;
	NPC * npc = new NPC(this->name, this->description, this->abilityList,
		(this->speed).roll(), (this->hitpoints).roll(), attackDamage,
		this->symbol, this->color, this->rarity, position);
	npc->setCountPointers(&this->howManyExist, &this->howManyDead);
	return npc;
}

void MonsterBlueprint::resetHowManyExist(){
	this->howManyExist = 0;
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
	out << *r.name << endl;
	out << *r.description;
	out << colorName << endl;
	out << r.speed << endl;
	for(size_t i = 0; i < r.abilityList.size();i++){
		out << r.abilityList[i];
	}
	out << endl;
	out << r.hitpoints << endl;
	out << r.attackDamage << endl;
	out << r.symbol << endl;
	out << r.rarity << endl;
	out << endl;
	return out;
}
