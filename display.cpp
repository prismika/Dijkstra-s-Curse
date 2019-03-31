#include <stdlib.h>
#include <iostream>
#include <limits.h>
#include <ncurses.h>
#include "display.h"
#include "mapElements.h"
#include "map.h"

#define SPACE_ABOVE_MAP 1

char * deathString = (char *) "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
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

static char getBlockVisual(BlockType type){
	switch(type){
		case rock:		return ' ';
		case floor:		return '.';
		case corridor:	return '#';
		case upstairs:	return '<';
		case downstairs:return '>';
		case bedrock: 	return '|';
		default:		return '!';
	}
}

int display_init(){
	// initscr();
	// curs_set(0);

	initscr();
	raw();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	mvprintw(1, 1, "Loading...");

	return 0;
}

int display_delete(){
	endwin();
	std::cout << deathString;
	return 0;
}

char getBlockVisual(BlockType type);

int display_map(Map * map){
	erase();
	int x,y;
	for(y=0;y<MAPHEIGHT;++y){
		for(x=0; x<MAPWIDTH; ++x){
			if(map_has_entity_at(map, x, y)){
				Entity curEnt;
				map_get_entity(map,x,y,&curEnt);
				mvaddch(y+SPACE_ABOVE_MAP,x,curEnt.symbol);
			}else{
				Block curBlock;
				map_getBlock(map,x,y,&curBlock);
				mvaddch(y+SPACE_ABOVE_MAP,x,getBlockVisual(curBlock.type));
			}
		}
	}
	refresh();
	return 0;
}

int display_map_foggy(Map * map){
	erase();
	int x,y;
	for(y=0;y<MAPHEIGHT;++y){
		for(x=0; x<MAPWIDTH; ++x){
			Coordinate curCoord;
			curCoord.x = x;
			curCoord.y = y;
			if(map_block_is_visible(map,curCoord) && map_has_entity_at(map, x, y)){
				Entity curEnt;
				map_get_entity(map,x,y,&curEnt);
				mvaddch(y+SPACE_ABOVE_MAP,x,curEnt.symbol);
			}else{
				Block curBlock;
				map_get_block_remembered(map,x,y,&curBlock);
				mvaddch(y+SPACE_ABOVE_MAP,x,getBlockVisual(curBlock.type));
			}
		}
	}
	refresh();
	return 0;
}

int display_distance_map(DistanceMap * dist){
	return 0;
}
int display_room_list(Room *roomList){
	return 0;
}
int display_population(Map * map){
	return 0;
}
int display_entity(Entity * ent){
	return 0;
}

int display_message(char * s){
	move(0,0);
	clrtoeol();
	mvprintw(0,1,s);
	return 0;
}


int display_population_list(Entity ** list, int listSize, Coordinate pcPos){
	erase();
	mvprintw(1,2,"Monsters");
	int i;
	for(i=0;i<listSize; i++){
		if(list[i]->dead) continue;
		int xDiff = list[i]->position.x - pcPos.x;
		int yDiff = list[i]->position.y - pcPos.y;
		mvprintw(i+2,2,"%c Relative position: %d, %d", list[i]->symbol, xDiff, yDiff);
	}
	refresh();
	return 0;
}

int display_population_list_offset(Map * map, int offset){
	Entity ** list = map_get_population_list(map);
	int listSize = map_get_population_size(map);
	Coordinate pcPos = map_get_pc_position(map);
	if(offset > listSize) return -1;
	display_population_list(list + offset, listSize - offset,pcPos);
	return 0;
}

int display_add_cursor(Coordinate cursorPos){
	mvprintw(cursorPos.y+1, cursorPos.x,"*");
	return 0;
}
