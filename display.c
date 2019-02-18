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
			Block curBlock;
			map_getBlock(map,x,y,&curBlock);
			output [y][x] = getBlockVisual(curBlock.type);
		}
	}
	output[map->pcPos.y][map->pcPos.x] ='@';

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
