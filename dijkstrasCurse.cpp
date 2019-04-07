#include <cstdlib>
#include <cstdbool>
#include <cstring>
#include <ctime>
#include <cstdio>
#include <iostream>
#include <vector>
#include "map.h"
#include "mapIO.h"
#include "mapElements.h"
#include "display.h"
#include "gameTypes.h"
#include "monsterParser.h"

long seed;
int nummon;

using namespace std;



//Strategy for strategy pattern
class IncomingCommand{
public:
	int (*execute)(void);
};

int executeSave(void);
int executeLoad(void);
int executeLoadSave(void);
int executeDistances(void);
int executeDefault(void);
int executeMonsters(void);
int executeItems(void);
int legacyFlag(void);

int parseCLI(int argc, char * argv[], IncomingCommand * command){
	class/*This would make more sense as a struct but it is
	required to be a class for the assignment*/
	{
	public:
		bool save;
		bool load;
		bool distances;
		bool seed;
		long theSeed;
		bool nummon;
		int monsterCount;
		bool monsters;
		bool items;
	}flags;
	flags.save=flags.load=flags.distances=flags.seed=flags.nummon=
	flags.monsters=flags.items=false;
	flags.theSeed = flags.monsterCount = 0;

	//Iterate through the command line input
	int i;
	for(i = 1; i < argc; ++i){
		if(!strcmp(argv[i],"--save")){
			flags.save = true;
		}else if(!strcmp(argv[i],"--load")){
			flags.load = true;
		}else if(!strcmp(argv[i],"--distances")){
			flags.distances = true;
		}else if(!strcmp(argv[i],"--seed")){
			if(i<argc-1){
				i++;
				flags.seed = true;
				flags.theSeed = strtol(argv[i],NULL,10);
			}else return -1;
		}else if(!strcmp(argv[i],"--nummon")){
			if(i<argc-1){
				i++;
				flags.nummon = true;
				flags.monsterCount = atoi(argv[i]);
			}else return -1;
		}else if(!strcmp(argv[i],"--monsters")){
			flags.monsters = true;
		}else if(!strcmp(argv[i],"--items")){
			flags.items = true;
		}else return -1;
	}

	//Unpack the temporary flags class and assign a strategy
	if(flags.seed){
		seed = flags.theSeed;
	}else{
		seed = time(0);
	}
	if(flags.nummon){
		nummon = flags.monsterCount;
	}else{
		nummon = 5;
	}
	if(flags.save && !flags.load){
		command->execute = executeSave;
	}else if(flags.load && !flags.save){
		command->execute = executeLoad;
	}else if(flags.load && flags.save){
		command->execute = executeLoadSave;
	}else if(flags.distances){
		command->execute = legacyFlag;//executeDistances;
	}else if(flags.monsters){
		command->execute = executeMonsters;
	}else if(flags.items){
		command->execute = executeItems;
	}else{
		command->execute = executeDefault;
	}
	return 0;
}

int main(int argc, char *argv[]){
	IncomingCommand whatsup;
	if(parseCLI(argc,argv,&whatsup)!=0){
		std::cout << "Incorrect usage\n";
		return -1;
	}
	//Execute the function that parseCLI chose
	whatsup.execute();
	return 0;
}

/*Typical game mode. Declared in gameTypes.h*/
int executeDefault(void){
	OriginalGameType game(seed,nummon);
	game.runGame();
	return 0;
}



/*--------------------Other modes no one cares about---------*/
int legacyFlag(){
	std::cout << "You used a legacy flag. "
	<< "That functionality is no longer supported :(\n";
	return 0;
}

int executeLoad(){
	Map theMap;
	map_init(&theMap);
	readFile(&theMap);
	display_map(&theMap);
	return 0;
}

int executeSave(){
	Map theMap;
	std::cout << "Seed:" << seed << std::endl;
	map_init(&theMap);
	generate_map(&theMap,seed);
	display_map(&theMap);
	writeFile(&theMap);
	return 0;
}

int executeLoadSave(){
	Map theMap;
	map_init(&theMap);
	readFile(&theMap);
	display_map(&theMap);
	writeFile(&theMap);
	return 0;
}

int executeDistances(){
	Map theMap;
	std::cout << "Seed:" << seed << std::endl;
	map_init(&theMap);
	generate_map(&theMap,seed);
	display_map(&theMap);
	DistanceMap dist;
	DistanceMap distWithTunneling;
	//Broken
	// get_distance_map(&theMap,theMap.pcPos,&dist);
	// get_distance_map_tunneling(&theMap,theMap.pcPos,&distWithTunneling);
	display_distance_map(&dist);
	display_distance_map(&distWithTunneling);
	return 0;
}

int executeMonsters(){
	vector<MonsterBlueprint> blueprintList;
	blueprintList = parser_load_monster_list();
	
	vector<MonsterBlueprint>::iterator iter;
	for(iter = blueprintList.begin(); iter !=blueprintList.end(); iter++){
		cout << *iter << endl;
	}
	
	return 0;
}

int executeItems(){
	// vector<ItemBlueprint> blueprintList;
	// parser_load_item_list(&blueprintList);
	
	// vector<ItemBlueprint>::iterator iter;
	// for(iter = blueprintList.begin(); iter !=blueprintList.end(); iter++){
	// 	cout << *iter << endl;
	// }
	return 0;
}