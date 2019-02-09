#include "mapElements.h"


struct Block block_create(enum BlockType blockType, uint8_t hardness){
	struct Block newBlock;
	newBlock.type = blockType;
	newBlock.hardness = hardness;
	return newBlock;
}