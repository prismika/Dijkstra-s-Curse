#include <stdio.h>
#include <stdlib.h>

#define MAPWIDTH 80
#define MAPHEIGHT 21
enum BlockType{
	rock,
	corridor,
	floor,
	upstairs,
	downstairs
};

struct Block{
	enum BlockType type;
};

struct Map{
	struct Block block[MAPWIDTH][MAPHEIGHT];
};


void printMap(struct Map map);
char getVisual(enum BlockType type);

int main(int argc, char *argv[]){
	struct Map map;
	int i,j;
	for(i=0;i<MAPHEIGHT;++i){
		for(j=0; j<MAPWIDTH; ++j){
			map.block[i][j].type = floor;
		}
	}
	printMap(map);
	return 0;
}


void printMap(struct Map map){
	int i,j;
	for(i=0;i<MAPHEIGHT;++i){
		for(j=0; j<MAPWIDTH; ++j){
			struct Block curBlock = map.block[i][j];
			char blockVisual[1];
			blockVisual[0] = getVisual(curBlock.type);

			printf(blockVisual);
		}
		printf("\n");
	}
}

char getVisual(enum BlockType type){
	switch(type){
		case rock:
			return ' ';
			break;
		case floor:
			return '.';
			break;
		default:
			return '!';
	}
	return '!';
}