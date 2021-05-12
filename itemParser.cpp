#include "itemParser.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "dice.h"
#include "populationElements.h"

#define VERSION_NAME "RLG327 OBJECT DESCRIPTION 1"


static void parse_type(string* s, ItemType* type){
	if(!s->compare("WEAPON")){
		*type = ITEM_TYPE_WEAPON;
	}else if(!s->compare("OFFHAND")){
		*type = ITEM_TYPE_OFFHAND;
	}else if(!s->compare("RANGED")){
		*type = ITEM_TYPE_RANGED;
	}else if(!s->compare("ARMOR")){
		*type = ITEM_TYPE_ARMOR;
	}else if(!s->compare("HELMET")){
		*type = ITEM_TYPE_HELMET;
	}else if(!s->compare("CLOAK")){
		*type = ITEM_TYPE_CLOAK;
	}else if(!s->compare("GLOVES")){
		*type = ITEM_TYPE_GLOVES;
	}else if(!s->compare("BOOTS")){
		*type = ITEM_TYPE_BOOTS;
	}else if(!s->compare("RING")){
		*type = ITEM_TYPE_RING;
	}else if(!s->compare("AMULET")){
		*type = ITEM_TYPE_AMULET;
	}else if(!s->compare("LIGHT")){
		*type = ITEM_TYPE_LIGHT;
	}else if(!s->compare("SCROLL")){
		*type = ITEM_TYPE_SCROLL;
	}else if(!s->compare("BOOK")){
		*type = ITEM_TYPE_BOOK;
	}else if(!s->compare("FLASK")){
		*type = ITEM_TYPE_FLASK;
	}else if(!s->compare("GOLD")){
		*type = ITEM_TYPE_GOLD;
	}else if(!s->compare("AMMUNITION")){
		*type = ITEM_TYPE_AMMUNITION;
	}else if(!s->compare("FOOD")){
		*type = ITEM_TYPE_FOOD;
	}else if(!s->compare("WAND")){
		*type = ITEM_TYPE_WAND;
	}else if(!s->compare("CONTAINER")){
		*type = ITEM_TYPE_CONTAINER;
	}
}

vector<ItemBlueprint> parser_load_item_list(){
	ifstream file;
	vector <ItemBlueprint> blueprintList;
	int l = strlen(getenv("HOME")) + strlen("/.rlg327/object_desc.txt") + 1; 
	char * filePath = (char *) malloc(l);
	strcpy(filePath,getenv("HOME"));
	strcat(filePath,"/.rlg327/object_desc.txt");
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
		nextItemDescription:
		class SectionsSeen{
		public:
			bool NAME = false;
			bool DESC = false;
			bool TYPE = false;
			bool COLOR = false;
			bool HIT = false;
			bool DAM = false;
			bool DODGE = false;
			bool DEF = false;
			bool WEIGHT = false;
			bool SPEED = false;
			bool ATTR = false;
			bool VAL = false;
			bool ART = false;
			bool RRTY = false;
		};
		SectionsSeen sectionsSeen;
		string name, desc;
		ItemType type = ITEM_TYPE_no_type;
		MonsterColor color = MONSTER_RED;
		Dice hit, damage, dodge, defence, weight, speed, attribute, value;
		bool artifact = false;
		int rarity = 10;
		while(getline(file, curLine)&&curLine.compare("BEGIN OBJECT")){
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
				if(sectionsSeen.NAME) goto nextItemDescription;
				sectionsSeen.NAME = true;
				name = curLine;
			}else if(!keyword.compare("DESC")){
				if(sectionsSeen.DESC) goto nextItemDescription;
				sectionsSeen.DESC = true;
				while(getline(file,curLine) && curLine.compare(".")){
					desc.append(curLine);
					desc.append("\n");
				}
			}else if(!keyword.compare("TYPE")){
				if(sectionsSeen.TYPE) goto nextItemDescription;
				sectionsSeen.TYPE = true;
				parse_type(&curLine,&type);
			}else if(!keyword.compare("COLOR")){
				if(sectionsSeen.COLOR) goto nextItemDescription;
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
			}else if(!keyword.compare("HIT")){
				if(sectionsSeen.HIT) goto nextItemDescription;
				sectionsSeen.HIT = true;
				Dice newDice(curLine);
				hit = newDice;
			}else if(!keyword.compare("DAM")){
				if(sectionsSeen.DAM) goto nextItemDescription;
				sectionsSeen.DAM = true;
				Dice newDice(curLine);
				damage = newDice;
			}else if(!keyword.compare("DODGE")){
				if(sectionsSeen.DODGE) goto nextItemDescription;
				sectionsSeen.DODGE = true;
				Dice newDice(curLine);
				dodge = newDice;
			}else if(!keyword.compare("DEF")){
				if(sectionsSeen.DEF) goto nextItemDescription;
				sectionsSeen.DEF = true;
				Dice newDice(curLine);
				defence = newDice;
			}else if(!keyword.compare("WEIGHT")){
				if(sectionsSeen.WEIGHT) goto nextItemDescription;
				sectionsSeen.WEIGHT = true;
				Dice newDice(curLine);
				weight = newDice;
			}else if(!keyword.compare("SPEED")){
				if(sectionsSeen.SPEED) goto nextItemDescription;
				sectionsSeen.SPEED = true;
				Dice newDice(curLine);
				speed = newDice;
			}else if(!keyword.compare("ATTR")){
				if(sectionsSeen.ATTR) goto nextItemDescription;
				sectionsSeen.ATTR = true;
				Dice newDice(curLine);
				attribute = newDice;
			}else if(!keyword.compare("VAL")){
				if(sectionsSeen.VAL) goto nextItemDescription;
				sectionsSeen.VAL = true;
				Dice newDice(curLine);
				value = newDice;
			}else if(!keyword.compare("ART")){
				if(sectionsSeen.ART) goto nextItemDescription;
				sectionsSeen.ART = true;
				artifact = !curLine.compare("TRUE");
			}else if(!keyword.compare("RRTY")){
				if(sectionsSeen.RRTY) goto nextItemDescription;
				sectionsSeen.RRTY = true;
				rarity = stoi(curLine);
			}
		}
// 		//Reached the end of this monster's description
		if(!sectionsSeen.NAME ||
			!sectionsSeen.DESC ||
			!sectionsSeen.TYPE ||
			!sectionsSeen.COLOR ||
			!sectionsSeen.HIT ||
			!sectionsSeen.DAM ||
			!sectionsSeen.DODGE ||
			!sectionsSeen.DEF ||
			!sectionsSeen.WEIGHT ||
			!sectionsSeen.SPEED ||
			!sectionsSeen.ATTR ||
			!sectionsSeen.VAL ||
			!sectionsSeen.ART ||
			!sectionsSeen.RRTY){
			goto nextItemDescription;
		}

		ItemBlueprint *blueprint = new ItemBlueprint(
			name, desc,
			type,
			color,
			hit,
			damage,
			dodge,
			defence,
			weight,
			speed,
			attribute,
			value,
			artifact,
			rarity);
		blueprintList.push_back(*blueprint);
	}
	fileFinished:
	return blueprintList;

}

ItemBlueprint::ItemBlueprint(){}

ItemBlueprint::ItemBlueprint(string &name,
	string &description,
	ItemType type,
	MonsterColor color,
	Dice &hit,
	Dice &damage,
	Dice &dodge,
	Dice &defence,
	Dice &weight,
	Dice &speed,
	Dice &attribute,
	Dice &value,
	bool artifact,
	int rarity)
	:name(name), description(description), type(type), color(color),
	hit(hit), damage(damage), dodge(dodge), defence(defence),
	weight(weight), speed(speed), attribute(attribute), value(value),
	artifact(artifact), rarity(rarity), howManyExist(0), howManyPickedUp(0){
}

int ItemBlueprint::getRarity(){
	return this->rarity;
}

Item * ItemBlueprint::build(){
	this->howManyExist++;
	Item * item = new Item(this->name,
	this->description,
	this->type,
	this->color,
	(this->hit).roll(),
	this->damage,
	(this->dodge).roll(),
	(this->defence).roll(),
	(this->weight).roll(),
	(this->speed).roll(),
	(this->attribute).roll(),
	(this->value).roll(),
	this->artifact,
	this->rarity);
	item->setHowManyExist(&this->howManyExist);
	item->setHowManyPickedUp(&this->howManyPickedUp);
	return item;
}

bool ItemBlueprint::isBuildable(){
	return !artifact
	|| ((howManyExist == 0) && (howManyPickedUp == 0));
}

void ItemBlueprint::resetHowManyExist(){
	this->howManyExist = 0;
}
// NPC * MonsterBlueprint::build(Coordinate position){
// 	return new NPC(&this->name, &this->description, this->abilityList,
// 		(this->speed).roll(), (this->hitpoints).roll(), attackDamage,
// 		this->symbol, this->color, this->rarity, position);
// }

ostream & operator<<(ostream &out, const ItemBlueprint &r){
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
	out << r.description <<endl;
	out << r.type << endl;
	out << colorName << endl;
	out << r.hit << endl;
	out << r.damage << endl;
	out << r.dodge << endl;
	out << r.defence << endl;
	out << r.weight << endl;
	out << r.speed << endl;
	out << r.attribute << endl;
	out << r.value << endl;
	out << r.artifact << endl;
	out << r.rarity << endl;
	out << endl;
	return out;
}
