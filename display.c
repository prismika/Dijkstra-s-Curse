
#include <stdlib.h>
#include <stdio.h>
#include "display.h"
#include "mapElements.h"


char getBlockVisual(enum BlockType type);

int display_map(Map * map){
	int i,j;
	for(i=0;i<MAPHEIGHT;++i){
		for(j=0; j<MAPWIDTH; ++j){
			Block curBlock;
			map_getBlock(map,j,i,&curBlock);
			//TODO Fix this shenannigans
			char blockVisual[1];
			blockVisual[0] = getBlockVisual(curBlock.type);

			printf(blockVisual);
		}
		printf("\n");
	}
	return 0;
}

int display_distance_map(DistanceMap * dist){
	int i, j;
	for(i=0; i<MAPHEIGHT; ++i){
		for(j=0; j<MAPWIDTH; ++j){
			int d = get_distance(dist,j,i);
			if(d==-1) printf(" ");
			else{
				printf("%d",d % 10);
			}
		}
		printf("\n");
	}

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
