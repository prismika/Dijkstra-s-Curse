#ifndef MAPELEMENTS_H
#define MAPELEMENTS_H
/*This is for housing all types related to a map, including
blocks, coordinates, and the map itself.*/

#include<stdint.h>
#include"mapElements.h"

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

typedef struct Block{
	BlockType type;
	uint8_t hardness;
} Block;

typedef struct Coordinate{
	int y;
	int x;
} Coordinate;

typedef struct Room{
	Coordinate position;
	int height;
	int width;
} Room;

/*It is assumed that the corridor goes horizontally from start
and vertically to end*/
typedef struct Corridor{
	Coordinate start;
	Coordinate midpoint;
	Coordinate end;
} Corridor;

typedef struct Map{
	Block block[MAPHEIGHT][MAPWIDTH];
	Room room[MAX_ROOM_COUNT+1];//DONT FORGET extra spot for sentinel value
	Coordinate pcPos;
	int width;
	int height;
} Map;

typedef struct{
	int dist[MAPHEIGHT][MAPWIDTH];
}DistanceMap;

Block block_create(BlockType blockType, uint8_t hardness);
void map_init(Map * map);
int map_getBlock(Map * map, int x, int y, Block * outputBlock);
int map_setBlock(Map * map, int x, int y, Block * inputBlock);
int get_distance(DistanceMap * dist, int x, int y);
void set_distance(DistanceMap * dist, int x, int y, int d);



#endif