#ifndef MAPELEMENTS_H
#define MAPELEMENTS_H

#include<stdint.h>

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

Block block_create(BlockType blockType, uint8_t hardness);

#endif