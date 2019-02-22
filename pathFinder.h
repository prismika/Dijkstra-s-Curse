#ifndef	PATHFINDER_H
#define PATHFINDER_H

#include "mapElements.h"
#include "map.h"

int get_distance_map(Map * map, Coordinate source, DistanceMap * dist);
int get_distance_map_tunneling(Map * map, Coordinate source, DistanceMap * dist);

#endif