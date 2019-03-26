#ifndef MAPELEMENTS_H
#define MAPELEMENTS_H
/*This is for housing all types related to a map, including
blocks, coordinates, and the map itself.*/

#include<stdint.h>
#include<stdbool.h>

#define MAPWIDTH 80
#define MAPHEIGHT 21
#define MIN_ROOM_COUNT 6
#define MAX_ROOM_COUNT 10

#define MIN_ROOM_WIDTH 4 //4
#define MIN_ROOM_HEIGHT 3 //3
#define MAX_ROOM_WIDTH 12 //Exclusive
#define MAX_ROOM_HEIGHT 8	//Exclusive

typedef enum BlockType{
	rock,
	bedrock,
	corridor,
	floor,
	upstairs,
	downstairs
} BlockType;

class Block{
public:
	BlockType type;
	uint8_t hardness;
};

class Coordinate{
public:
	int y;
	int x;
};

class Room{
public:
	Coordinate position;
	int height;
	int width;
};

/*It is assumed that the corridor goes horizontally from start
and vertically to end*/
class Corridor{
public:
	Coordinate start;
	Coordinate midpoint;
	Coordinate end;
};


class DistanceMap{
public:
	int dist[MAPHEIGHT][MAPWIDTH];
};

Block block_create(BlockType blockType, uint8_t hardness);
void distance_map_init(DistanceMap * dist);
int get_distance(DistanceMap * dist, int x, int y);
void set_distance(DistanceMap * dist, int x, int y, int d);
bool room_is_sentinel(Room room);
int distLInf(Coordinate v1, Coordinate v2);



#endif