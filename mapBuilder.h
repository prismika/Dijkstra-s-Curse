#ifndef MAPBUILDER_H
#define MAPBUILDER_H

#include "mapElements.h"
#include "map.h"

void generate_map(Map * map, long seed);
void placeNewRoom(Map *map, Room *room);


#endif