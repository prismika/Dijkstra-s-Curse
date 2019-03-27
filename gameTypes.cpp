#include "gameTypes.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include <iostream>
#include "map.h"
#include "mapIO.h"
#include "mapElements.h"
#include "mapBuilder.h"
#include "pathFinder.h"
#include "display.h"
#include "mapPopulator.h"
#include "turnMaster.h"
#include "inputCollector.h"

OriginalGameType::OriginalGameType(int seed, int nummon){
	this->seed = seed;
	this->nummon = nummon;
	std::cout << "Seed:" << seed;
	//Init things
	gameMode = new MovementGameMode;
	display_init();
	inputState_init(&inputState);
}

int OriginalGameType::runGame(){
	//Generate!
	init_level();
	//Main loop
	while(true){
		int status = gameMode->execute_mode_actions(this);
		if(status == -1){
			quit_game();
			return 0;
		}
		//Did the PC die???
		if(map_pc_is_dead(&theMap)){
			//Press f to pay respects
			handle_death();
			quit_game();
			return 0;
		}
		// switch(mode){
		// 	case mode_map:{
		// 		//Get next turn from turnMaster
		// 		nextTurnEnt = turnmaster_get_next_turn(&turnMaster);
		// 		if(nextTurnEnt == NULL) return -1;
		// 		//If the next turn belongs to an NPC, they gotta do what the gotta do
		// 		if(nextTurnEnt->isPC){
		// 			display_map(&theMap);
		// 		}else{
		// 			//Calculate attempted move of entity with current turn
		// 			Coordinate moveCoord;
		// 			DistanceMap * distNonTunnel = map_get_distance_map_non_tunneling(&theMap);
		// 			DistanceMap * distTunnel = map_get_distance_map_tunneling(&theMap);
		// 			entity_get_move(nextTurnEnt, distNonTunnel, distTunnel, &moveCoord);
		// 			//Tell the map our intended move
		// 			map_move_entity(&theMap, nextTurnEnt, moveCoord);
		// 		}
		// 		//Did the PC die???
		// 		if(map_pc_is_dead(&theMap)){
		// 			//Press f to pay respects
		// 			handle_death();
		// 			quit_game();
		// 			return 0;
		// 		}

		// 	}break;

		// 	//This is the mode in which the user can see the list of monsters
		// 	case mode_monster_list:{
		// 		display_message((char *)"We are in monster list mode");
		// 		display_population_list_offset(&theMap, scrollOffset+1);
		// 	}break;
		// }
	}
	return 0;
}


void OriginalGameType::init_level(){
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
void OriginalGameType::delete_level(){
	//delete map will go here
}
/*deinits everything and outputs the death message to the console.*/
void OriginalGameType::quit_game(){
	delete_level();
	display_delete();
}
/*Returns -1 if we are quitting the game*/
int OriginalGameType::interpret_pc_input(Entity * pc, InputState * inState){
	InputType inputType = inputState_get_last(inState);
	//Mode-independent checks
	if(inputType == input_quit){
		quit_game();
		return -1;
	}


	//Mode-dependent things
	switch(mode){
		case mode_map:
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
				display_message((char*)"Your hero stumbles as he tries to take stairs that do not exist");
				return 0;
			}
			delete_level();
			init_level();
		}else if(inputType == input_mlist){
			mode = mode_monster_list;
		}
		break;


		case mode_monster_list:
		switch(inputType){
			case input_escape:
				mode = mode_map;
				break;
			case input_mlist_down:
				scrollOffset = scrollOffset < map_get_population_size(&theMap)-2?
					scrollOffset + 1 :
					map_get_population_size(&theMap) - 2;
				break;
			case input_mlist_up:
				scrollOffset = scrollOffset > 0 ?
					scrollOffset - 1 :
					0;
				break;

			default:break;
		}
	}
	return 0;
}

void OriginalGameType::handle_death(void){
	display_map(&theMap);
	display_message((char *)"Press any key to continue");
	getch();//This should be a call to inputCollector
}










//------------------------MODES---------------------------
int MovementGameMode::execute_mode_actions(OriginalGameType * game){
	//Get next turn from turnMaster
	nextTurnEnt = turnmaster_get_next_turn(&game->turnMaster);
	if(nextTurnEnt == NULL) return -2; //Turnmaster should never be empty
	//If the next turn belongs to an NPC, they gotta do what the gotta do
	if(nextTurnEnt->isPC){
		display_map(&game->theMap);
		//Get user input [Blocking call]
		inputState_update(&game->inputState);//TODO make mode-dependent
		//Interpret and execute input with helper function
		int interpretStatus = interpret_pc_input(nextTurnEnt, &game->inputState, game);
		if(interpretStatus == -1){
			return -1;
		}
	}else{
		//Calculate attempted move of entity with current turn
		Coordinate moveCoord;
		DistanceMap * distNonTunnel = map_get_distance_map_non_tunneling(&game->theMap);
		DistanceMap * distTunnel = map_get_distance_map_tunneling(&game->theMap);
		entity_get_move(nextTurnEnt, distNonTunnel, distTunnel, &moveCoord);
		//Tell the map our intended move
		map_move_entity(&game->theMap, nextTurnEnt, moveCoord);
	}
	return 0;
}

int MovementGameMode::interpret_pc_input(Entity * pc, InputState * inState, OriginalGameType * game){
	InputType inputType = inputState_get_last(inState);
	if(inputType == input_quit){
		return -1;
	}
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
		Coordinate newCoord = map_move_entity(&game->theMap, pc, moveCoord);
		//Update distance maps
		DistanceMap * dist = map_get_distance_map_non_tunneling(&game->theMap);
		get_distance_map(&game->theMap,newCoord,dist);
		dist = map_get_distance_map_tunneling(&game->theMap);
		get_distance_map_tunneling(&game->theMap,newCoord,dist);
		return 0;
	}//Check if the input was a stair movement
	else if(inputState_is_stair(inState)){
		Block pcBlock;
		map_getBlock(&game->theMap,pc->position.x,pc->position.y,&pcBlock);
		if(!(pcBlock.type == upstairs || pcBlock.type == downstairs)){
			display_message((char*)"Your hero stumbles as he tries to take stairs that do not exist");
			return 0;
		}
		game->delete_level();
		game->init_level();
	}
	return 0;
}
