#ifndef DISPLAY_H
#define DISPLAY_H
#include "mapElements.h"
#include "map.h"

int display_map(Map * map);
int display_distance_map(DistanceMap * dist);
int display_room_list(Room *roomList);
int display_population(Map * map);

#endif
