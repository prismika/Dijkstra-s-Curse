#ifndef	DIJ_H
#define DIJ_H

#include "mapElements.h"

int get_distance_map(Map * map, Coordinate source, DistanceMap * dist);
int get_distance_map_tunneling(Map * map, Coordinate source, DistanceMap * dist);

#endif