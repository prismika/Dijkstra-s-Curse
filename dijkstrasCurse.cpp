#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "map.h"
#include "mapIO.h"
#include "mapElements.h"
#include "display.h"
#include "gameTypes.h"

long seed;
int nummon;



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

	//Unpack the temporary flags class and assign a strategy
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
	IncomingCommand whatsup;
	if(parseCLI(argc,argv,&whatsup)!=0){
		printf("Incorrect usage\n");
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
	printf("You used a legacy flag. ");
	printf("That functionality is no longer supported :(\n");
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
	printf("Seed:%ld\n", seed);
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
	printf("Seed:%ld\n", seed);
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