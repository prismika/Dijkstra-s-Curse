#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>
#include "map.h"
#include "mapIO.h"
#include "mapElements.h"
#include "mapBuilder.h"
#include "pathFinder.h"
#include "display.h"
#include "mapPopulator.h"
#include "TurnMaster.h"
#include "inputCollector.h"
#include "display.h"

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
	//Execute the function that parseCLI chose
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

static Coordinate interpret_pc_input(Entity * pc, InputState * inState){
	InputType inputType = inputState_get_last(inState);
	Coordinate returnCoord = pc->position;
	switch(inputType){
		case input_upleft:
			returnCoord.y--;
			returnCoord.x--;
		break;
		case input_up:
			returnCoord.y--;
		break;
		case input_upright:
			returnCoord.x++;
			returnCoord.y--;
		break;
		case input_right:
			returnCoord.x++;
		break;
		case input_downright:
			returnCoord.y++;
			returnCoord.x++;
		break;
		case input_down:
			returnCoord.y++;
		break;
		case input_downleft:
			returnCoord.y++;
			returnCoord.x--;
		break;
		case input_left:
			returnCoord.x--;
		break;

		

		case input_null:
		default:
		break;
	}
	return returnCoord;
}

static void handleDeath(void){
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
"                                 (  .      )\n"
"                             )           (              )\n"
"                                   .  '   .   '  .  '  .\n"
"                          (    , )       (.   )  (   ',    )\n"
"                           .' ) ( . )    ,  ( ,     )   ( .\n"
"                        ). , ( .   (  ) ( , ')  .' (  ,    )\n"
"                       (_,) . ), ) _) _,')  (, ) '. )  ,. (' )\n"
"                      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"
"                   Your hero has fought valliantly,\n"
"				   but has succumb to Dijkstra's Curse\n\n\n\n\n\n"
	);
}

int executeDefault(){
	//Display seed for posterity
	printf("Seed:%ld\n", seed);
	//Init things
	display_init();
	InputState inputState;
	inputState_init(&inputState);
	//Generate!
	generate_map(&theMap,seed);
	populate_map(&theMap,nummon);
	TurnMaster turnMaster;
	turnmaster_init(&turnMaster);
	//Get population matrix from map
	Entity **populationMatrix = map_get_population_matrix(&theMap);
	//Give population to turnmaster
	turnmaster_fill_from_matrix(&turnMaster, populationMatrix);
	display_map(&theMap);

	//Main loop
	while(true){
		//Get next turn from turnMaster
		Entity * nextTurnEnt;
		nextTurnEnt = turnmaster_get_next_turn(&turnMaster);
		//turnMaster should never be empty
		if (nextTurnEnt == NULL){
			return -1;
		}
		
		//Special things happen if current entity is the PC
		if(nextTurnEnt->isPC){
			//Get input
			inputState_update(&inputState);
			//Interpret input with helper function 
			Coordinate moveCoord = interpret_pc_input(nextTurnEnt, &inputState);
			//Move the PC accordingly
			Coordinate newCoord = map_move_entity(&theMap, nextTurnEnt, moveCoord);
			//Update distance maps
			DistanceMap * dist = map_get_distance_map_non_tunneling(&theMap);
			get_distance_map(&theMap,newCoord,dist);
			dist = map_get_distance_map_tunneling(&theMap);
			get_distance_map_tunneling(&theMap,newCoord,dist);
			//Wait a bit, then display the map
			usleep(250000);
			display_map(&theMap);
		}else{//Extra special things happen if current entity is not the PC
			//Calculate attempted move of entity with current turn
			Coordinate moveCoord;
			DistanceMap * distNonTunnel = map_get_distance_map_non_tunneling(&theMap);
			DistanceMap * distTunnel = map_get_distance_map_tunneling(&theMap);
			entity_get_move(nextTurnEnt, distNonTunnel, distTunnel, &moveCoord);
			//Make the move
			map_move_entity(&theMap, nextTurnEnt, moveCoord);
		}
		//If the PC is dead
		if(map_pc_is_dead(&theMap)){
			//deinit what needs to be deinited
			display_delete();
			//Press f to pay respects
			handleDeath();
			//End the loop
			return 0;
		}
	}
	return 0;
}
