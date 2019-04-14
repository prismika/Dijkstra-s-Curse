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
	pc = new PC();
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
			quit_game(false);
			return 0;
		}
		//Did the PC die???
		if(map_pc_is_dead(&theMap)){
			//Press f to pay respects
			handle_death();
			quit_game(false);
			return 0;
		}
		if(theMap.isBossDead()){
			handle_death();
			quit_game(true);
			return 0;
		}
	}
	return 0;
}

void OriginalGameType::init_level(){
	map_init(&theMap);
	generate_map(&theMap,++seed);
	mapPopulator.populate(&theMap,pc,nummon);
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
void OriginalGameType::quit_game(bool win){
	delete_level();
	display_delete(win);
}

void OriginalGameType::handle_death(void){
	display_map(&theMap);
	display_message((char *)"Press any key to continue");
	getch();//This should be a call to inputCollector
}










//------------------------MODES---------------------------
MovementGameMode::MovementGameMode(){
	fog = true;
}

int MovementGameMode::execute_mode_actions(OriginalGameType * game){
	//Assume it's the PC's turn
	fog ?
		display_map_foggy(&game->theMap):
		display_map(&game->theMap);
	//Get user input [Blocking call]
	inputState_update(&game->inputState);//TODO make mode-dependent
	//Interpret and execute input with helper function
	int interpretStatus = interpret_pc_input(game->pc, &game->inputState, game);
	if(interpretStatus == -1){
		return -1;
	}
	if(interpretStatus != 1){
		//PC didn't move. Don't advance the turn. We're done here.
		return 0;
	}

	//Run the turnmaster like a madman
	Entity * nextTurnEnt = turnmaster_get_next_turn(&game->turnMaster);
	if(nextTurnEnt == NULL) return -2; //Turnmaster should never be empty
	while(!nextTurnEnt->isPC){
		//Calculate attempted move of entity with current turn
		Coordinate moveCoord;
		DistanceMap * distNonTunnel = map_get_distance_map_non_tunneling(&game->theMap);
		DistanceMap * distTunnel = map_get_distance_map_tunneling(&game->theMap);
		entity_get_move((NPC *)nextTurnEnt, distNonTunnel, distTunnel, &moveCoord);
		//Tell the map our intended move
		map_move_entity(&game->theMap, nextTurnEnt, moveCoord);
		//Did the pc die?
		if(map_pc_is_dead(&game->theMap)){
			//Press f to pay respects
			game->handle_death();
			game->quit_game(false);
			return 0;
		}
		nextTurnEnt = turnmaster_get_next_turn(&game->turnMaster);
		if(nextTurnEnt == NULL) return -2; //Turnmaster should never be empty
	}
	return 0;
}

int MovementGameMode::interpret_pc_input(PC * pc, InputState * inState, OriginalGameType * game){
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
		return 1;
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
		game->gameMode = new TeleportGameMode(game->pc->position);
	}else if(inputType == input_fog){
		fog = !fog;
	}else if(inputType == input_wear){
		game->gameMode = new InventoryWearGameMode;
	}else if(inputType == input_i){
		game->gameMode = new InventoryDisplayGameMode;
	}else if(inputType == input_e){
		game->gameMode = new EquipmentDisplayGameMode;
	}else if(inputType == input_t){
		game->gameMode = new EquipmentRemoveGameMode;
	}else if(inputType == input_x){
		game->gameMode = new InventoryDestroyGameMode;
	}else if(inputType == input_d){
		game->gameMode = new InventoryDropGameMode;
	}else if(inputType == input_I){
		game->gameMode = new InventoryInspectGameMode;
	}else if(inputType == input_L){
		game->gameMode = new EntityInspectGameMode(game->pc->position);
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
		game->quit_game(false);
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




TeleportGameMode::TeleportGameMode(Coordinate pcPos){
	cursorPos = pcPos;
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
		game->quit_game(false);
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

InventoryWearGameMode::InventoryWearGameMode(){}

int InventoryWearGameMode::execute_mode_actions(OriginalGameType * game){
	// display_population_list_offset(&game->theMap, scrollOffset+1);
	PC * pc = game->pc;
	display_inventory(pc);
	//Get user input [Blocking call]
	inputState_update(&game->inputState);
	int interpretStatus = interpret_pc_input(&game->inputState, game);
	if(interpretStatus == -1){
		return -1;
	}
	return 0;
}

int InventoryWearGameMode::interpret_pc_input(InputState * inState, OriginalGameType * game){
	InputType inputType = inputState_get_last(inState);
	//Mode-independent checks
	if(inputType == input_quit){
		game->quit_game(false);
		return -1;
	}
	int equipSlot;
	//This switch statement is awful because I designed my input system weirdly
	switch(inputType){
		case input_escape:
			game->gameMode = new MovementGameMode;
			return 0;
		case input_0:
			equipSlot = 0;
			game->gameMode = new MovementGameMode;
			break;
		case input_downleft:
			equipSlot = 1;
			game->gameMode = new MovementGameMode;
			break;
		case input_down:
			equipSlot = 2;
			game->gameMode = new MovementGameMode;
			break;
		case input_downright:
			equipSlot = 3;
			game->gameMode = new MovementGameMode;
			break;
		case input_left:
			equipSlot = 4;
			game->gameMode = new MovementGameMode;
			break;
		case input_rest:
			equipSlot = 5;
			game->gameMode = new MovementGameMode;
			break;
		case input_right:
			equipSlot = 6;
			game->gameMode = new MovementGameMode;
			break;
		case input_upleft:
			equipSlot = 7;
			game->gameMode = new MovementGameMode;
			break;
		case input_up:
			equipSlot = 8;
			game->gameMode = new MovementGameMode;
			break;
		case input_upright:
			equipSlot = 9;
			game->gameMode = new MovementGameMode;
			break;
		default:
			equipSlot = -1;
			break;
	}
	if(equipSlot != -1){
		game->pc->equip(equipSlot);
	}
	return 0;
}

InventoryDisplayGameMode::InventoryDisplayGameMode(){}

int InventoryDisplayGameMode::execute_mode_actions(OriginalGameType * game){
	PC * pc = game->pc;
	display_inventory(pc);
	//Get user input [Blocking call]
	inputState_update(&game->inputState);
	int interpretStatus = interpret_pc_input(&game->inputState, game);
	if(interpretStatus == -1){
		return -1;
	}
	return 0;
}

int InventoryDisplayGameMode::interpret_pc_input(InputState * inState, OriginalGameType * game){
	InputType inputType = inputState_get_last(inState);
	//Mode-independent checks
	if(inputType == input_quit){
		game->quit_game(false);
		return -1;
	}
	//This switch statement is awful because I designed my input system weirdly
	switch(inputType){
		case input_escape:
			game->gameMode = new MovementGameMode;
			return 0;
		//Paste in code here for looking at stats
		default:
			break;
	}
	return 0;
}



EquipmentDisplayGameMode::EquipmentDisplayGameMode(){}

int EquipmentDisplayGameMode::execute_mode_actions(OriginalGameType * game){
	PC * pc = game->pc;
	display_equipment(pc);
	//Get user input [Blocking call]
	inputState_update(&game->inputState);
	int interpretStatus = interpret_pc_input(&game->inputState, game);
	if(interpretStatus == -1){
		return -1;
	}
	return 0;
}

int EquipmentDisplayGameMode::interpret_pc_input(InputState * inState, OriginalGameType * game){
	InputType inputType = inputState_get_last(inState);
	//Mode-independent checks
	if(inputType == input_quit){
		game->quit_game(false);
		return -1;
	}
	//This switch statement is awful because I designed my input system weirdly
	switch(inputType){
		case input_escape:
			game->gameMode = new MovementGameMode;
			return 0;
		//Paste in code here for looking at stats
		default:
			break;
	}
	return 0;
}




EquipmentRemoveGameMode::EquipmentRemoveGameMode(){}

int EquipmentRemoveGameMode::execute_mode_actions(OriginalGameType * game){
	PC * pc = game->pc;
	display_equipment(pc);
	//Get user input [Blocking call]
	inputState_update(&game->inputState);
	int interpretStatus = interpret_pc_input(&game->inputState, game);
	if(interpretStatus == -1){
		return -1;
	}
	return 0;
}

int EquipmentRemoveGameMode::interpret_pc_input(InputState * inState, OriginalGameType * game){
	InputType inputType = inputState_get_last(inState);
	//Mode-independent checks
	if(inputType == input_quit){
		game->quit_game(false);
		return -1;
	}
	//This switch statement is awful because I designed my input system weirdly
	int removeSlot;
	switch(inputType){
		case input_escape:
			game->gameMode = new MovementGameMode;
			return 0;
		case input_a:
			removeSlot = 0;
			break;
		case input_downleft:
			removeSlot = 1;
			break;
		case input_c:
			removeSlot = 2;
			break;
		case input_d:
			removeSlot = 3;
			break;
		case input_e:
			removeSlot = 4;
			break;
		case input_fog:
			removeSlot = 5;
			break;
		case input_g:
			removeSlot = 6;
			break;
		case input_left:
			removeSlot = 7;
			break;
		case input_i:
			removeSlot = 8;
			break;
		case input_down:
			removeSlot = 9;
			break;
		case input_up:
			removeSlot = 10;
			break;
		case input_right:
			removeSlot = 11;
			break;
		default:
			return -2;
	}
	if(game->pc->unequip(removeSlot)>=0){
		game->gameMode = new MovementGameMode;
	}
	return 0;
}

InventoryDestroyGameMode::InventoryDestroyGameMode(){}

int InventoryDestroyGameMode::execute_mode_actions(OriginalGameType * game){
	// display_population_list_offset(&game->theMap, scrollOffset+1);
	PC * pc = game->pc;
	display_inventory(pc);
	//Get user input [Blocking call]
	inputState_update(&game->inputState);
	int interpretStatus = interpret_pc_input(&game->inputState, game);
	if(interpretStatus == -1){
		return -1;
	}
	return 0;
}

int InventoryDestroyGameMode::interpret_pc_input(InputState * inState, OriginalGameType * game){
	InputType inputType = inputState_get_last(inState);
	//Mode-independent checks
	if(inputType == input_quit){
		game->quit_game(false);
		return -1;
	}
	int destroySlot;
	//This switch statement is awful because I designed my input system weirdly
	switch(inputType){
		case input_escape:
			game->gameMode = new MovementGameMode;
			return 0;
		case input_0:
			destroySlot = 0;
			game->gameMode = new MovementGameMode;
			break;
		case input_downleft:
			destroySlot = 1;
			game->gameMode = new MovementGameMode;
			break;
		case input_down:
			destroySlot = 2;
			game->gameMode = new MovementGameMode;
			break;
		case input_downright:
			destroySlot = 3;
			game->gameMode = new MovementGameMode;
			break;
		case input_left:
			destroySlot = 4;
			game->gameMode = new MovementGameMode;
			break;
		case input_rest:
			destroySlot = 5;
			game->gameMode = new MovementGameMode;
			break;
		case input_right:
			destroySlot = 6;
			game->gameMode = new MovementGameMode;
			break;
		case input_upleft:
			destroySlot = 7;
			game->gameMode = new MovementGameMode;
			break;
		case input_up:
			destroySlot = 8;
			game->gameMode = new MovementGameMode;
			break;
		case input_upright:
			destroySlot = 9;
			game->gameMode = new MovementGameMode;
			break;
		default:
			destroySlot = -1;
			break;
	}
	if(destroySlot != -1){
		game->pc->destroy(destroySlot);
	}
	return 0;
}

InventoryDropGameMode::InventoryDropGameMode(){}

int InventoryDropGameMode::execute_mode_actions(OriginalGameType * game){
	// display_population_list_offset(&game->theMap, scrollOffset+1);
	PC * pc = game->pc;
	display_inventory(pc);
	//Get user input [Blocking call]
	inputState_update(&game->inputState);
	int interpretStatus = interpret_pc_input(&game->inputState, game);
	if(interpretStatus == -1){
		return -1;
	}
	return 0;
}

int InventoryDropGameMode::interpret_pc_input(InputState * inState, OriginalGameType * game){
	InputType inputType = inputState_get_last(inState);
	//Mode-independent checks
	if(inputType == input_quit){
		game->quit_game(false);
		return -1;
	}
	int destroySlot;
	//This switch statement is awful because I designed my input system weirdly
	switch(inputType){
		case input_escape:
			game->gameMode = new MovementGameMode;
			return 0;
		case input_0:
			destroySlot = 0;
			game->gameMode = new MovementGameMode;
			break;
		case input_downleft:
			destroySlot = 1;
			game->gameMode = new MovementGameMode;
			break;
		case input_down:
			destroySlot = 2;
			game->gameMode = new MovementGameMode;
			break;
		case input_downright:
			destroySlot = 3;
			game->gameMode = new MovementGameMode;
			break;
		case input_left:
			destroySlot = 4;
			game->gameMode = new MovementGameMode;
			break;
		case input_rest:
			destroySlot = 5;
			game->gameMode = new MovementGameMode;
			break;
		case input_right:
			destroySlot = 6;
			game->gameMode = new MovementGameMode;
			break;
		case input_upleft:
			destroySlot = 7;
			game->gameMode = new MovementGameMode;
			break;
		case input_up:
			destroySlot = 8;
			game->gameMode = new MovementGameMode;
			break;
		case input_upright:
			destroySlot = 9;
			game->gameMode = new MovementGameMode;
			break;
		default:
			destroySlot = -1;
			break;
	}
	Coordinate pcCoord = game->pc->position;
	if(destroySlot != -1 && !map_has_item_at(&game->theMap,pcCoord.x, pcCoord.y)){
		Item * item = game->pc->destroy(destroySlot);
		game->theMap.placeItem(item,pcCoord);
	}
	return 0;
}

InventoryInspectGameMode::InventoryInspectGameMode(){}

int InventoryInspectGameMode::execute_mode_actions(OriginalGameType * game){
	// display_population_list_offset(&game->theMap, scrollOffset+1);
	PC * pc = game->pc;
	display_inventory(pc);
	//Get user input [Blocking call]
	inputState_update(&game->inputState);
	int interpretStatus = interpret_pc_input(&game->inputState, game);
	if(interpretStatus == -1){
		return -1;
	}
	return 0;
}

int InventoryInspectGameMode::interpret_pc_input(InputState * inState, OriginalGameType * game){
	InputType inputType = inputState_get_last(inState);
	//Mode-independent checks
	if(inputType == input_quit){
		game->quit_game(false);
		return -1;
	}
	int inspectSlot;
	//This switch statement is awful because I designed my input system weirdly
	switch(inputType){
		case input_escape:
			game->gameMode = new MovementGameMode;
			return 0;
		case input_0:
			inspectSlot = 0;
			game->gameMode = new MovementGameMode;
			break;
		case input_downleft:
			inspectSlot = 1;
			game->gameMode = new MovementGameMode;
			break;
		case input_down:
			inspectSlot = 2;
			game->gameMode = new MovementGameMode;
			break;
		case input_downright:
			inspectSlot = 3;
			game->gameMode = new MovementGameMode;
			break;
		case input_left:
			inspectSlot = 4;
			game->gameMode = new MovementGameMode;
			break;
		case input_rest:
			inspectSlot = 5;
			game->gameMode = new MovementGameMode;
			break;
		case input_right:
			inspectSlot = 6;
			game->gameMode = new MovementGameMode;
			break;
		case input_upleft:
			inspectSlot = 7;
			game->gameMode = new MovementGameMode;
			break;
		case input_up:
			inspectSlot = 8;
			game->gameMode = new MovementGameMode;
			break;
		case input_upright:
			inspectSlot = 9;
			game->gameMode = new MovementGameMode;
			break;
		default:
			inspectSlot = -1;
			break;
	}
	if(game->pc->hasInventoryItem(inspectSlot)){
		display_item_description(game->pc->getInventoryItem(inspectSlot));
		getch();
		game->gameMode = new MovementGameMode;
	}
	return 0;
}


EntityInspectGameMode::EntityInspectGameMode(Coordinate pcPos){
	cursorPos = pcPos;
}

int EntityInspectGameMode::execute_mode_actions(OriginalGameType * game){
	display_map(&game->theMap);
	display_add_cursor(cursorPos);
	inputState_update(&game->inputState);
	int interpretStatus = interpret_pc_input(&game->inputState, game);
	if(interpretStatus == -1){
		return -1;
	}
	return 0;
}

int EntityInspectGameMode::interpret_pc_input(InputState * inState, OriginalGameType * game){
	InputType inputType = inputState_get_last(inState);
	//Mode-independent check
	if(inputType == input_quit){
		game->quit_game(false);
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

		case input_t:{
			Entity ent;
			if(map_has_entity_at(&game->theMap, cursorPos.x, cursorPos.y)){
				map_get_entity(&game->theMap, cursorPos.x, cursorPos.y, &ent);
				display_entity_description(&ent);
				getch();
			}
			game->gameMode = new MovementGameMode;
		}break;

		default:
		break;
	}
	cursorPos.x = std::max(std::min(cursorPos.x,MAPWIDTH-2),1);
	cursorPos.y = std::max(std::min(cursorPos.y,MAPHEIGHT-2),1);
	return 0;
}
