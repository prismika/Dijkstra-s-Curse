#include "monsterParser.h"
#include <cstdlib>

class dice{
private:
	int base;
	int count;
	int sides;
public:
	dice(int base, int count, int sides){
		this->base = base;
		this->count = count;
		this->sides = sides;
	}
	dice(std::string){
		this->base = 2;
		this->count = 2;
		this->sides = 3;
	}
	int roll(){
		int sum = base;
		for(int i = 0; i < count; i++){
			sum += rand()%sides + 1;
		}
		return sum;
	}
};

MonsterBlueprint * parser_load_monster_list(int * listSize){
	MonsterBlueprint * blueprint = (MonsterBlueprint*) malloc(sizeof(MonsterBlueprint*)*2);
	*listSize = 2;
	return blueprint;
}