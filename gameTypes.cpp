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
	}
	return 0;
}

void OriginalGameType::init_level(){
	map_init(&theMap);
	generate_map(&theMap,++seed);
	populate_map(&theMap,nummon);
	map_update_remembered(&theMap);
	turnmaster_init(&turnMaster);
	//Get population matrix from map to fill turnmaster
	Entity **populationMatrix = map_get_population_matrix(&theMap);
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

void OriginalGameType::handle_death(void){
	display_map(&theMap);
	display_message((char *)"Press any key to continue");
	getch();//This should be a call to inputCollector
}










//------------------------MODES---------------------------
MovementGameMode::MovementGameMode(){
	fog = true;
	nextTurnEnt = NULL;
}

int MovementGameMode::execute_mode_actions(OriginalGameType * game){
	//Get next turn from turnMaster
	nextTurnEnt = turnmaster_get_next_turn(&game->turnMaster);
	if(nextTurnEnt == NULL) return -2; //Turnmaster should never be empty
	//If the next turn belongs to an NPC, they gotta do what the gotta do
	if(nextTurnEnt->isPC){
		fog ?
			display_map_foggy(&game->theMap) :
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
		//Update remembered map
		map_update_remembered(&game->theMap);
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
	}else if(inputType == input_mlist){
		game->gameMode = new ListGameMode;
	}else if(inputType == input_teleport){
		game->gameMode = new TeleportGameMode;
	}else if(inputType == input_fog){
		fog = !fog;
	}
	return 0;
}




ListGameMode::ListGameMode(){
	scrollOffset = 0;
}

int ListGameMode::execute_mode_actions(OriginalGameType * game){
	display_population_list_offset(&game->theMap, scrollOffset+1);
	//Get user input [Blocking call]
	inputState_update(&game->inputState);//TODO make mode-dependent
	//Interpret and execute input with helper function
	int interpretStatus = interpret_pc_input(&game->inputState, game);
	if(interpretStatus == -1){
		return -1;
	}
	return 0;
}

int ListGameMode::interpret_pc_input(InputState * inState, OriginalGameType * game){
	InputType inputType = inputState_get_last(inState);
	//Mode-independent checks
	if(inputType == input_quit){
		game->quit_game();
		return -1;
	}

	switch(inputType){
		case input_escape:
			game->gameMode = new MovementGameMode;
			break;
		case input_mlist_down:
			scrollOffset = scrollOffset < map_get_population_size(&game->theMap)-2?
				scrollOffset + 1 :
				map_get_population_size(&game->theMap) - 2;
			break;
		case input_mlist_up:
			scrollOffset = scrollOffset > 0 ?
				scrollOffset - 1 :
				0;
			break;
		default:break;
	}
	return 0;
}




TeleportGameMode::TeleportGameMode(){
	cursorPos = {1,1};
}

int TeleportGameMode::execute_mode_actions(OriginalGameType * game){
	display_map(&game->theMap);
	display_add_cursor(cursorPos);
	inputState_update(&game->inputState);
	int interpretStatus = interpret_pc_input(&game->inputState, game);
	if(interpretStatus == -1){
		return -1;
	}
	return 0;
}

int TeleportGameMode::interpret_pc_input(InputState * inState, OriginalGameType * game){
	InputType inputType = inputState_get_last(inState);
	//Mode-independent check
	if(inputType == input_quit){
		game->quit_game();
		return -1;
	}
	switch(inputType){
		case input_right:
		cursorPos.x++;
		break;
		case input_upright:
		cursorPos.x++;
		cursorPos.y--;
		break;
		case input_up:
		cursorPos.y--;
		break;
		case input_upleft:
		cursorPos.x--;
		cursorPos.y--;
		break;
		case input_left:
		cursorPos.x--;
		break;
		case input_downleft:
		cursorPos.x--;
		cursorPos.y++;
		break;
		case input_down:
		cursorPos.y++;
		break;
		case input_downright:
		cursorPos.x++;
		cursorPos.y++;
		break;

		case input_teleport:
		map_teleport_entity(&game->theMap, map_get_pc(&game->theMap), cursorPos);
		game->gameMode = new MovementGameMode;
		break;

		case input_random:{
			BlockType canChoose[] 	= {floor, corridor};
			int canChooseSize 		= 2;
			Coordinate teleportTo;
			map_choose_random_block(&game->theMap, canChoose, canChooseSize, &teleportTo);
			map_teleport_entity(&game->theMap, map_get_pc(&game->theMap), teleportTo);
			map_update_remembered(&game->theMap);
			game->gameMode = new MovementGameMode;
		}
		break;

		default:
		break;
	}
	cursorPos.x = std::max(std::min(cursorPos.x,MAPWIDTH-2),1);
	cursorPos.y = std::max(std::min(cursorPos.y,MAPHEIGHT-2),1);
	return 0;
}

