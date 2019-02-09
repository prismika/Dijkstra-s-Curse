#ifndef MAPELEMENTS_H
#define MAPELEMENTS_H

#include<stdint.h>

//TODO typedefs
enum BlockType{
	rock,
	bedrock,
	corridor,
	floor,
	upstairs,
	downstairs
};
// typedef enum BlockType BlockType;

struct Block{
	enum BlockType type;
	uint8_t hardness;
};
// typedef struct Block Block;

struct Coordinate{
	int y;
	int x;
};
// typedef struct Coordinate Coordinate;

struct Room{
	struct Coordinate position;
	int height;
	int width;
};
// typedef struct Room Room;


/*It is assumed that the corridor goes horizontally from start
and vertically to end*/
struct Corridor{
	struct Coordinate start;
	struct Coordinate midpoint;
	struct Coordinate end;
};
//typedef struct Corridor Corridor;

struct Block block_create(enum BlockType blockType, uint8_t hardness);

#endif