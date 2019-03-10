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
#include "turnMaster.h"
#include "inputCollector.h"
#include "display.h"

long seed;
Map theMap;
TurnMaster turnMaster;
InputState inputState;
int nummon;

char * deathString = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
"                                 (  .      )\n"
"                             )           (              )\n"
"                                   .  '   .   '  .  '  .\n"
"                          (    , )       (.   )  (   ',    )\n"
"                           .' ) ( . )    ,  ( ,     )   ( .\n"
"                        ). , ( .   (  ) ( , ')  .' (  ,    )\n"
"                       (_,) . ), ) _) _,')  (, ) '. )  ,. (' )\n"
"                      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"
"                   Your hero has fought valliantly,\n"
"				   but has succumb to Dijkstra's Curse\n\n\n\n\n\n";

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




static void init_level(){
	map_init(&theMap);
	generate_map(&theMap,++seed);
	populate_map(&theMap,nummon);
	turnmaster_init(&turnMaster);
	//Get population matrix from map
	Entity **populationMatrix = map_get_population_matrix(&theMap);
	//Give population to turnmaster
	turnmaster_fill_from_matrix(&turnMaster, populationMatrix);
}

/*Deinits stuff from the level*/
static void delete_level(){
	//delete map will go here
}
/*deinits everything and prints the death message.*/
static void quit_game(){
	delete_level();
	display_delete();
	printf(deathString);
}
/*Returns -1 if quitting*/
static int interpret_pc_input(Entity * pc, InputState * inState){
	InputType inputType = inputState_get_last(inState);
	if(inputState_is_movement(inState)){
		Coordinate moveCoord = pc->position;
		switch(inputType){
			case input_upleft:
				moveCoord.y--;
				moveCoord.x--;
			break;
			case input_up:
				moveCoord.y--;
			break;
			case input_upright:
				moveCoord.x++;
				moveCoord.y--;
			break;
			case input_right:
				moveCoord.x++;
			break;
			case input_downright:
				moveCoord.y++;
				moveCoord.x++;
			break;
			case input_down:
				moveCoord.y++;
			break;
			case input_downleft:
				moveCoord.y++;
				moveCoord.x--;
			break;
			case input_left:
				moveCoord.x--;
			break;
			default:break;
		}
		//Move the PC accordingly
		Coordinate newCoord = map_move_entity(&theMap, pc, moveCoord);
		//Update distance maps
		DistanceMap * dist = map_get_distance_map_non_tunneling(&theMap);
		get_distance_map(&theMap,newCoord,dist);
		dist = map_get_distance_map_tunneling(&theMap);
		get_distance_map_tunneling(&theMap,newCoord,dist);
		return 0;
	}//Check if the input was a stair movement
	else if(inputState_is_stair(inState)){
		Block pcBlock;
		map_getBlock(&theMap,pc->position.x,pc->position.y,&pcBlock);
		if(!(pcBlock.type == upstairs || pcBlock.type == downstairs)){
			display_message("Your hero stumbles as he tries to take stairs that do not exist");
			return 0;
		}
		// TODO delete_level
		init_level();
	}else if(inputType == input_mlist){
		//display map
		display_message("I can't display the monster list yet :(");
	}else if(inputType == input_quit){
		quit_game();
		return -1;
	}
	return 0;
}

static void handle_death(void){
	display_map(&theMap);
	display_message("Press any key to continue");
	getch();
}

int executeDefault(){
	//Display seed for posterity
	printf("Seed:%ld\n", seed);
	//Init things
	display_init();
	inputState_init(&inputState);
	//Generate!
	init_level();
	//Main loop
	while(true){
		//Get next turn from turnMaster
		Entity * nextTurnEnt;
		nextTurnEnt = turnmaster_get_next_turn(&turnMaster);
		//turnMaster should never be empty
		if (nextTurnEnt == NULL) return -1;
		//Special things happen if current entity is the PC
		if(nextTurnEnt->isPC){
			display_map(&theMap);
			//Get user input [Blocking call]
			inputState_update(&inputState);
			display_message("");
			//Interpret and execute input with helper function 
			int interpretStatus = interpret_pc_input(nextTurnEnt, &inputState);
			if(interpretStatus == -1) return 0;
		}else{//Extra special things happen if current entity is not the PC
			//Calculate attempted move of entity with current turn
			Coordinate moveCoord;
			DistanceMap * distNonTunnel = map_get_distance_map_non_tunneling(&theMap);
			DistanceMap * distTunnel = map_get_distance_map_tunneling(&theMap);
			entity_get_move(nextTurnEnt, distNonTunnel, distTunnel, &moveCoord);
			//Tell the map our intended move
			map_move_entity(&theMap, nextTurnEnt, moveCoord);
		}
		//If the PC is dead
		if(map_pc_is_dead(&theMap)){
			//Press f to pay respects
			handle_death();
			quit_game();
			return 0;
		}
	}
	return 0;
}






/*--------------------Other modes no one cares about---------*/
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