#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "display.h"
#include "mapElements.h"


char getBlockVisual(enum BlockType type);

int display_map(Map * map){
	int x,y;
	char output[MAPHEIGHT][MAPWIDTH];
	for(y=0;y<MAPHEIGHT;++y){
		for(x=0; x<MAPWIDTH; ++x){
			if(map_has_entity_at(map, x, y)){
				Entity curEnt;
				map_get_entity(map,x,y,&curEnt);
				output[y][x] = curEnt.symbol;
			}else{
				Block curBlock;
				map_getBlock(map,x,y,&curBlock);
				output [y][x] = getBlockVisual(curBlock.type);
			}
		}
	}

	for(y=0;y<MAPHEIGHT;++y){
		for(x=0; x<MAPWIDTH; ++x){
			printf("%c",output[y][x]);
		}
		printf("\n");
	}
	printf("\n");

	return 0;
}

int display_distance_map(DistanceMap * dist){
	int x, y;
	char output[MAPHEIGHT][MAPWIDTH];

	for(y=0; y<MAPHEIGHT; ++y){
		for(x=0; x<MAPWIDTH; ++x){
			char out;
			int d = get_distance(dist,x,y);
			switch(d){
				case INT_MAX:out = ' ';
				break;
				case 0:		out = '@';
				break;
				default:	out = (d % 10) + '0';
				break;

			}
			if(		y==0 || y==MAPHEIGHT -1
				||	x==0 || x==MAPWIDTH -1){
				out = 'X';
			}
			output[y][x] = out;
		}
	}

	for(y=0; y<MAPHEIGHT; ++y){
		for(x=0; x<MAPWIDTH; ++x){
			printf("%c",output[y][x]);
		}
		printf("\n");
	}
	printf("\n");

	return 0;
}

int display_room_list(Room *roomList){
	printf("Rooms:\n");

	int i;
	for(i=0;i<MAX_ROOM_COUNT;++i){
		Room curRoom = roomList[i];
		printf("\tRoom %d: X|%d Y|%d W|%d H|%d\n",
			i,curRoom.position.x,curRoom.position.y,
			curRoom.width,curRoom.height);
	}

	return 0;
}

int display_population(Map * map){
	int x,y;
	for(y=0; y<MAPHEIGHT; ++y){
		for(x=0; x<MAPWIDTH; ++x){
			if(map_has_entity_at(map,x,y)){
				Entity curEnt;
				map_get_entity(map,x,y,&curEnt);
				printf("%c X|%d Y|%d\n", curEnt.symbol,curEnt.position.x, curEnt.position.y);
				if(curEnt.position.x != x || curEnt.position.y != y){
					printf("(at %d   %d in the population map!!!!!!!!!)\n", x,y);
				}
			}
		}
	}
	return 0;
}


char getBlockVisual(enum BlockType type){
	switch(type){
		case rock:		return ' ';
		case floor:		return '.';
		case corridor:	return '#';
		case upstairs:	return '<';
		case downstairs:return '>';
		case bedrock: 	return '|';
		default:		return '!';
	}
}
