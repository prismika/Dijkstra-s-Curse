#include <stdlib.h>
#include <endian.h>
#include <iostream>
#include <string.h>
#include "mapBuilder.h"
#include "mapIO.h"
#include "mapElements.h"
#include "map.h"

#define FILE_PATH "/.rlg327/dungeon"
//"/.rlg327/jerBear/06.rlg327" //
FILE * fp;


int openFile(char * mode){
	int l = strlen(getenv("HOME")) + strlen(FILE_PATH) + 1; 
	char * filePath = (char *) malloc(l);
	strcpy(filePath,getenv("HOME"));
	strcat(filePath,FILE_PATH);
	if(!(fp=fopen(filePath,mode))){
		std::cerr << "Failed to open file.\n";
		return -1;
	}
	free(filePath);
	return 0;
}
int closeFile(){
	if(fclose(fp)!=0){
		std::cerr << "Failed to close file\n";
		return -1;
	}
	return 0;
}
int readFile(Map * newMap){
	// openFile("rb");
	// char data1, data2;
	// fread(&data1,1,1,fp);
	// fread(&data2,1,1,fp);
	char uselessInfoIn[16];
	uint32_t fileSizeIn;
	uint8_t pcPosXIn;
	uint8_t pcPosYIn;
	uint8_t hardnessesIn[MAPHEIGHT][MAPWIDTH];
	uint16_t roomCountIn;
	uint8_t * roomSpecsIn;//Remember to malloc AND free this sucker
	uint16_t upStairCountIn;
	uint8_t * upStairSpecsIn;//And this one
	uint16_t downStairCountIn;
	uint8_t * downStairSpecsIn;//And this one!


	openFile((char *)"rb");
	//Header things
	fread(&uselessInfoIn,1,16,fp);
	for(int i=0; i<16; ++i){
	}

	//File size
	fread(&fileSizeIn,4,1,fp);
	fileSizeIn = be32toh(fileSizeIn);

	//PC Coordinate
	fread(&pcPosXIn,1,1,fp);
	fread(&pcPosYIn,1,1,fp);

	//Hardness
	int i,j;
	for(i=0; i<MAPHEIGHT; ++i){
		for(j=0; j<MAPWIDTH; ++j){
			fread(&hardnessesIn[i][j],1,1,fp);
		}
	}

	//Room count
	fread(&roomCountIn,2,1,fp);
	roomCountIn = be16toh(roomCountIn);

	//Room specs
	roomSpecsIn = (uint8_t *) malloc(4*roomCountIn);
	for(i=0; i<4*roomCountIn; ++i){
		fread(&roomSpecsIn[i],1,1,fp);
	}

	//Up stairs count
	fread(&upStairCountIn,2,1,fp);
	upStairCountIn = be16toh(upStairCountIn);

	//Up stairs specs
	upStairSpecsIn = (uint8_t *) malloc(upStairCountIn*2);
	for(i=0;i<upStairCountIn*2;++i){
		fread(&upStairSpecsIn[i],1,1,fp);
	}


	//Down stairs count
	fread(&downStairCountIn,2,1,fp);
	downStairCountIn = be16toh(downStairCountIn);

	//Down stairs specs
	downStairSpecsIn = (uint8_t *) malloc(downStairCountIn*2);
	for(i=0;i<downStairCountIn*2;++i){
		fread(&downStairSpecsIn[i],1,1,fp);
	}


	//----Construct map from extracted values----

	map_init(newMap);
	//PC Position
	Coordinate pcPos;
	pcPos.x=pcPosXIn;
	pcPos.y=pcPosYIn;
	PC pc();
	pc.setPosition(pcPos);
	map_set_entity(newMap, pcPos.x,pcPos.y, &pc);
	//Hardness matrix
	for(i=0;i<MAPHEIGHT;++i){
		for(j=0;j<MAPWIDTH;++j){
			enum BlockType newType;
			uint8_t hardness = hardnessesIn[i][j];
			if(hardness == 0){
				newType = corridor;
			}else if(hardness == 255){
				newType = bedrock;
			}else{
				newType = rock;
			}
			Block createBlock = block_create(newType,hardness);
			map_setBlock(newMap,j,i,&createBlock);
		}
	}
	//Rooms
	int k;
	for(i=0; i<roomCountIn; ++i){
		Room newRoom;
		newRoom.position.x = roomSpecsIn[4*i];
		newRoom.position.y = roomSpecsIn[4*i+1];
		newRoom.width = roomSpecsIn[4*i+2];
		newRoom.height = roomSpecsIn[4*i+3];
		newMap->room[i] = newRoom;

		for(j=0; j<newRoom.height; ++j){
			for(k=0; k<newRoom.width; ++k){
				placeNewRoom(newMap, &newRoom);
			}
		}
	}

	for(i=0; i<upStairCountIn; ++i){
		Block blockToStairs;
		map_getBlock(newMap,upStairSpecsIn[2*i],
			upStairSpecsIn[2*i+1],
			&blockToStairs);
		blockToStairs.type = upstairs;
		map_setBlock(newMap,upStairSpecsIn[2*i],
			upStairSpecsIn[2*i+1],
			&blockToStairs);
	}

	for(i=0; i<downStairCountIn; ++i){
		Block blockToStairs;
		map_getBlock(newMap,downStairSpecsIn[2*i],
			downStairSpecsIn[2*i+1],
			&blockToStairs);
		blockToStairs.type = downstairs;
		map_setBlock(newMap,downStairSpecsIn[2*i],
			downStairSpecsIn[2*i+1],
			&blockToStairs);
	}

	free(roomSpecsIn);
	free(upStairSpecsIn);
	free(downStairSpecsIn);

	// displayMap(&newMap);

	closeFile();
	return 0;
}
int writeFile(Map * theMap){
	openFile((char *) "wb");

	//Things to write
	char uselessInfoOut[17] = "RLG327-S2019\0\0\0\0";
	uint32_t fileSizeOut;						//Calculate (Last)
	uint8_t pcPosXOut;
	uint8_t pcPosYOut;
	uint8_t hardnessesOut[MAPHEIGHT][MAPWIDTH];	//Fill (Map passover 1)
	uint16_t roomCountOut = 0;	//Count (Room passover)
	// uint8_t * roomSpecsOut;		//Fill  (Room passover)
	uint16_t upStairCountOut=0;	//Count (Map passover 1)
	// uint8_t * upStairSpecsOut;	//Fill  (Map passover 2)
	uint16_t downStairCountOut=0;//Count (Map passover 1)
	// uint8_t * downStairSpecsOut;//Fill  (Map passover 2)

	//Map passover 1
	int i,j;
	for(i=0;i<MAPHEIGHT;++i){
		for(j=0;j<MAPWIDTH;++j){
			Block curBlock;
			map_getBlock(theMap,j,i,&curBlock);
			hardnessesOut[i][j]=curBlock.hardness;
			if(curBlock.type == upstairs){
				upStairCountOut++;
			}else if(curBlock.type == downstairs){
				downStairCountOut++;
			}
			if(map_has_entity_at(theMap,j,i)){
				Entity curEnt;
				map_get_entity(theMap, j, i, &curEnt);
				//TODO This line was written by satan
				if(curEnt.symbol == '@'){
					pcPosXOut = j;
					pcPosYOut = i;
				}
			}
		}
	}
	uint8_t * upStairSpecsOut;
	uint8_t  * downStairSpecsOut;
	if(!(upStairSpecsOut   = (uint8_t *) malloc(2*upStairCountOut))
	|| !(downStairSpecsOut = (uint8_t *) malloc(2*downStairCountOut))){
		std::cout << "Failed to malloc.\n";
		return -1;
	}


	//Map passover 2
	int upIndex = 0;
	int downIndex = 0;
	for(i=0;i<MAPHEIGHT;++i){
		for(j=0;j<MAPWIDTH;++j){
			Block curBlock;
			map_getBlock(theMap,j,i,&curBlock);
			if(curBlock.type == upstairs){
				upStairSpecsOut[upIndex++] = j;
				upStairSpecsOut[upIndex++] = i;
			}else if(curBlock.type == downstairs){
				downStairSpecsOut[downIndex++] = j;
				downStairSpecsOut[downIndex++] = i;
			}
		}
	}

	//Room passover 1
	for(i=0; i<MAX_ROOM_COUNT; ++i){
		if(room_is_sentinel(theMap->room[i])){
			break;
		}
		roomCountOut++;
	}
	uint8_t * roomSpecsOut = (uint8_t *) malloc(4*roomCountOut);

	//Room passover 2
	for(i=0;i<roomCountOut;++i){
		roomSpecsOut[4*i]=theMap->room[i].position.x;
		roomSpecsOut[4*i+1]=theMap->room[i].position.y;
		roomSpecsOut[4*i+2]=theMap->room[i].width;
		roomSpecsOut[4*i+3]=theMap->room[i].height;
	}

	fileSizeOut = 1708
	+ 4*roomCountOut
	+ 2*upStairCountOut
	+ 2*downStairCountOut;




	//--------Write calculated values to file-----------

	for(i=0;i<16; ++i){
		fwrite(uselessInfoOut+i,1,1,fp);
	}

	uint32_t tmp32 = htobe32(fileSizeOut);
	fwrite(&tmp32,1,4,fp);

	fwrite(&pcPosXOut,1,1,fp);
	fwrite(&pcPosYOut,1,1,fp);

	for(i=0; i<MAPHEIGHT; ++i){
		for(j=0; j<MAPWIDTH; ++j){
			fwrite(&hardnessesOut[i][j],1,1,fp);
		}
	}

	uint16_t tmp16 = htobe16(roomCountOut);
	fwrite(&tmp16,1,2,fp);


	for(i=0;i<4*roomCountOut;++i){
		fwrite(&roomSpecsOut[i],1,1,fp);
	}

	tmp16 = htobe16(upStairCountOut);
	fwrite(&tmp16,1,2,fp);
	for(i=0;i<upStairCountOut;++i){
		fwrite(&upStairSpecsOut[2*i],1,1,fp);
		fwrite(&upStairSpecsOut[2*i+1],1,1,fp);
	}

	tmp16 = htobe16(downStairCountOut);
	fwrite(&tmp16,1,2,fp);
	for(i=0;i<downStairCountOut;++i){
		fwrite(&downStairSpecsOut[2*i],1,1,fp);
		fwrite(&downStairSpecsOut[2*i+1],1,1,fp);
	}

	free(upStairSpecsOut);
	free(downStairSpecsOut);
	free(roomSpecsOut);
	closeFile();

	return 0;
}