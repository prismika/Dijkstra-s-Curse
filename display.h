#ifndef DISPLAY_H
#define DISPLAY_H
#include "mapElements.h"
#include "populationElements.h"
#include "map.h"

int display_init();
int display_delete(bool win);
int display_map(Map * map);
int display_map_foggy(Map * map);
int display_distance_map(DistanceMap * dist);
int display_room_list(Room *roomList);
int display_population(Map * map);
int display_entity(Entity * ent);
int display_message(char * s);
int display_population_list(Entity ** list, int listSize, Coordinate pcPos);
int display_population_list_offset(Map * map, int offset);
int display_add_cursor(Coordinate cursorPos);
int display_inventory(PC *pc);
int display_equipment(PC *pc);
int display_item_description(Item * item);
int display_entity_description(Entity * ent);

#endif
