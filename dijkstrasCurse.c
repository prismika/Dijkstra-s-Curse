#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "map.h"
#include "mapIO.h"
#include "mapElements.h"
#include "mapBuilder.h"
#include "pathFinder.h"
#include "display.h"
#include "mapPopulator.h"

long seed;
Map theMap;
int nummon;

//Strategy for strategy pattern
struct IncomingCommand{
	int (*execute)(void);
};

int executeSave(void);
int executeLoad(void);
int executeLoadSave(void);
int executeDistances(void);
int executeDefault(void);
int legacyFlag(void);

int parseCLI(int argc, char * argv[], struct IncomingCommand * command){
	struct
	{
		bool save;
		bool load;
		bool distances;
		bool seed;
		long theSeed;
		bool nummon;
		int monsters;
	}flags;
	flags.save=flags.load=flags.distances=flags.seed=flags.nummon=false;
	flags.theSeed = flags.monsters = 0;

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
				printf("Seed should be %ld\n", flags.theSeed);
			}else return -1;
		}else if(!strcmp(argv[i],"--nummon")){
			if(i<argc-1){
				i++;
				flags.nummon = true;
				flags.monsters = atoi(argv[i]);
			}else return -1;
		}else return -1;
	}

	//Unpack the flags struct and assign a strategy
	if(flags.seed){
		seed = flags.theSeed;
	}else{
		seed = time(0);
	}
	if(flags.nummon){
		nummon = flags.monsters;
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
	}else{
		command->execute = executeDefault;
	}
	return 0;
}

int main(int argc, char *argv[]){
	struct IncomingCommand whatsup;
	if(parseCLI(argc,argv,&whatsup)!=0){
		printf("Incorrect usage\n");
		return -1;
	}
	map_init(&theMap);
	whatsup.execute();
	return 0;
}

int legacyFlag(){
	printf("You used a legacy flag. ");
	printf("That functionality is no longer supported :(\n");
	return 0;
}

int executeLoad(){
	readFile(&theMap);
	display_map(&theMap);
	return 0;
}

int executeSave(){
	printf("Seed:%ld\n", seed);
	generate_map(&theMap,seed);
	display_map(&theMap);
	writeFile(&theMap);
	return 0;
}

int executeLoadSave(){
	readFile(&theMap);
	display_map(&theMap);
	writeFile(&theMap);
	return 0;
}

int executeDistances(){
	printf("Seed:%ld\n", seed);
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

int executeDefault(){
	printf("Seed:%ld\n", seed);
	generate_map(&theMap,seed);
	populate_map(&theMap,nummon);
	//Create and init turnmaster
	display_map(&theMap);
	while(true){
		usleep(250000);
		// Entity nextTurnEnt;
		//	turnmaster_get_turn()
		// Coordinate moveCoord;
		// entity_get_move(&nextTurnEnt, &theMap, &moveCoord);
		// map_move_entity(&theMap, &nextTurnEnt, moveCoord);
		//if it is the pc....
		// Coordinate source = ...;
		// DistanceMap * dist = map_get_distance_map_non_tunneling(&theMap);
		// get_distance_map(&theMap,source,dist);
		// dist = map_get_distance_map_tunneling(&theMap);
		// get_distance_map_tunneling(&theMap,source,dist);
		display_map(&theMap);
	}
	return 0;
}
