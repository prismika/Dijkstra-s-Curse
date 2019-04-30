#include <stdlib.h>
#include <iostream>
#include <limits.h>
#include <ncurses.h>
#include <sstream>
#include "display.h"
#include "mapElements.h"
#include "map.h"

using namespace std;

#define SPACE_ABOVE_MAP 1
#define COLOR_MEMORY COLOR_YELLOW

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
char * winString = (char *) "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
"                      __   __               __        __ _         _ \n"
"                      \\ \\ / /  ___   _   _  \\ \\      / /(_)  __ _ | |\n"
"                       \\ V /  / _ \\ | | | |  \\ \\ /\\ / / | | / _` || |\n"
"                        | |  | (_) || |_| |   \\ V  V /  | || | | ||_|\n"
"                        |_|   \\___/ |_.__/     \\_/\\_/   |_||_| |_|(_)\n\n"
"                             You have escaped Dijkstra's Curse.\n"
"                         Dijkstra sheds a single tear. He is proud.\n"
"                 \"You found your path,\" he whispers. \"You found your path.\"\n"
"\n\n\n\n\n\n";
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

static int getNcursesColor(MonsterColor color){
	switch(color){
	case MONSTER_RED:
		return COLOR_RED;
	case MONSTER_GREEN:
		return COLOR_GREEN;
	case MONSTER_BLUE:
		return COLOR_BLUE;
	case MONSTER_CYAN:
		return COLOR_CYAN;
	case MONSTER_YELLOW:
		return COLOR_YELLOW;
	case MONSTER_MAGENTA:
		return COLOR_MAGENTA;
	case MONSTER_WHITE:
		return COLOR_WHITE;
	case MONSTER_BLACK:
		return COLOR_BLACK;
	default:
		return -1;
	}
}

int display_init(){
	// initscr();
	// curs_set(0);

	initscr();
	start_color();
	raw();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	mvprintw(1, 1, "Loading...");
	init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
	init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
	init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
	init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);

	return 0;
}

int display_delete(bool win){
	endwin();
	cout << (win ? winString : deathString) <<endl;
	return 0;
}

int display_map(Map * map){
	int x,y;
	for(y=0;y<MAPHEIGHT;++y){
		for(x=0; x<MAPWIDTH; ++x){
			if(map_has_entity_at(map, x, y)){
				Entity curEnt;
				map_get_entity(map,x,y,&curEnt);
				int color = getNcursesColor(curEnt.getColor());
				attron(COLOR_PAIR(color));
				mvaddch(y+SPACE_ABOVE_MAP,x,curEnt.symbol);
				attroff(COLOR_PAIR(color));
			}else if(map_has_item_at(map,x,y)){
				Coordinate curCoord;
				curCoord.x = x;
				curCoord.y = y;
				Item * item = map->getItemAt(curCoord);
				int color = getNcursesColor(item->getColor());
				attron(COLOR_PAIR(color));
				mvaddch(y+SPACE_ABOVE_MAP,x,item->getSymbol());
				attroff(COLOR_PAIR(color));
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
	int x,y;
	for(y=0;y<MAPHEIGHT;++y){
		for(x=0; x<MAPWIDTH; ++x){
			Coordinate curCoord;
			curCoord.x = x;
			curCoord.y = y;
			bool isVisible = map_block_is_visible(map,curCoord);
			if(isVisible && map_has_entity_at(map, x, y)){
				Entity curEnt;
				map_get_entity(map,x,y,&curEnt);
				int color = getNcursesColor(curEnt.getColor());
				attron(COLOR_PAIR(color));
				mvaddch(y+SPACE_ABOVE_MAP,x,curEnt.symbol);
				attroff(COLOR_PAIR(color));
			}else if(isVisible && map_has_item_at(map,x,y)){
				Coordinate curCoord;
				curCoord.x = x;
				curCoord.y = y;
				Item * item = map->getItemAt(curCoord);
				int color = getNcursesColor(item->getColor());
				attron(COLOR_PAIR(color));
				mvaddch(y+SPACE_ABOVE_MAP,x,item->getSymbol());
				attroff(COLOR_PAIR(color));
			}else{
				Block curBlock;
				map_get_block_remembered(map,x,y,&curBlock);
				if(isVisible){
					mvaddch(y+SPACE_ABOVE_MAP,x,getBlockVisual(curBlock.type));
				}else{
					attron(COLOR_PAIR(COLOR_YELLOW));
					mvaddch(y+SPACE_ABOVE_MAP,x,getBlockVisual(curBlock.type));
					attroff(COLOR_PAIR(COLOR_YELLOW));
				}
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

int display_message(const char * s){
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

int display_inventory(PC *pc){
	Item * item;
	int i;
	for(i = 0; i < INVENTORY_SIZE; i++){
		mvprintw(SPACE_ABOVE_MAP + 1 + i*2, 30, "%d) ", i);
		if(pc->hasInventoryItem(i)){
			item = pc->getInventoryItem(i);
			printw(item->getName()->c_str());
			mvprintw(SPACE_ABOVE_MAP + 2 + i*2, 31, "");
			printw("att: %s ", item->getDamage().toString().c_str());
			printw("sp: %d ", item->getSpeed());
		}
	}
	return 0;
}


int display_equipment(PC *pc){
	Item * item;
	int i;
	int itemSlot = 0;
	for(i = 0; i < EQUIPMENT_SIZE/2; i++){
		mvprintw(SPACE_ABOVE_MAP + 2 + 2*i,3,"%c) ", itemSlot +'a');
		if(pc->hasEquipmentItem(itemSlot)){
			item = pc->getEquipmentItem(itemSlot);
			printw(item->getName()->c_str());
			mvprintw(SPACE_ABOVE_MAP + 2 + i*2+1, 3, "");
			printw("att: %s ", item->getDamage().toString().c_str());
			printw("sp: %d ", item->getSpeed());
		}
		itemSlot++;
	}
	for(i=0; i<(EQUIPMENT_SIZE+1)/2; i++){
		mvprintw(SPACE_ABOVE_MAP + 2 + 2*i,40,"%c) ", itemSlot +'a');
		if(pc->hasEquipmentItem(itemSlot)){
			item = pc->getEquipmentItem(itemSlot);
			printw(item->getName()->c_str());
			mvprintw(SPACE_ABOVE_MAP + 2 + i*2+1, 40, "");
			printw("att: %s ", item->getDamage().toString().c_str());
			printw("sp: %d ", item->getSpeed());
		}
		itemSlot++;
	}
	return 0;
}

int display_item_description(Item * item){
	erase();
	stringstream ss(*item->getDescription());
	string line;
	int i = SPACE_ABOVE_MAP+6;
	while(getline(ss,line,'\n')){
		mvprintw(i++,3, line.c_str());
	}
	mvprintw(i+1, 3, "Press any key to return...");
	return 0;
}

int display_entity_description(Entity * ent){
	erase();
	int i = SPACE_ABOVE_MAP+2;
	mvprintw((i+=2), 3, ent->getName()->c_str());
	stringstream ss(*ent->getDescription());
	string line;
	while(getline(ss,line,'\n')){
		mvprintw(i++,3, line.c_str());
	}
	mvprintw(i+1, 3, "Press any key to return...");
	return 0;
}


int display_pc_stats(PC * pc){
	mvprintw(SPACE_ABOVE_MAP + MAPHEIGHT, 1, "");
	printw("hp: %d ", pc->getHP());
	printw("sp: %d ", pc->getSpeed());
	return 0;
}
