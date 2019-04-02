#include "monsterParser.h"
#include <cstdlib>

MonsterBlueprint * parser_load_monster_list(int * listSize){
	MonsterBlueprint * blueprint = (MonsterBlueprint*) malloc(sizeof(MonsterBlueprint*)*2);
	*listSize = 2;
	return blueprint;
}