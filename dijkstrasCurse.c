#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "mapIO.h"
#include "mapElements.h"
#include "mapBuilder.h"
#include "pathFinder.h"
#include "display.h"

enum IncomingCommand{
	nothing,
	save,
	load,
	load_save,
	distances
};


int parseCLI(int argc, char * argv[], enum IncomingCommand * command, long * seed);


//-----------I/O--------------------

int main(int argc, char *argv[]){

	long seed;
	enum IncomingCommand whatsup;
	if(parseCLI(argc,argv,&whatsup,&seed)!=0){
		printf("Incorrect usage\n");
		return -1;
	}
	Map theMap;
	map_init(&theMap);
	switch(whatsup){
		case nothing:
		printf("Seed:%ld\n", seed);
		generate_map(&theMap,seed);
		display_map(&theMap);
		return 0;

		case load:
		readFile(&theMap);
		display_map(&theMap);
		return 0;

		case save:
		printf("Seed:%ld\n", seed);
		generate_map(&theMap,seed);
		display_map(&theMap);
		writeFile(&theMap);
		return 0;

		case load_save:
		readFile(&theMap);
		display_map(&theMap);
		writeFile(&theMap);
		return 0;

		case distances:
		printf("Seed:%ld\n", seed);
		generate_map(&theMap,seed);
		display_map(&theMap);
		DistanceMap dist;
		DistanceMap distWithTunneling;
		get_distance_map(&theMap,theMap.pcPos,&dist);
		get_distance_map_tunneling(&theMap,theMap.pcPos,&distWithTunneling);
		display_distance_map(&dist);
		display_distance_map(&distWithTunneling);
		return 0;

		default:
		printf("That command is unfortunately not supported right now.\n");
		return 0;
	}
}


int parseCLI(int argc, char * argv[], enum IncomingCommand * command, long * seed){
	*seed = time(0);
	if(argc > 3){
		return -1;
	}
	if(argc == 3){
		*command = load_save;
	}else if(argc == 1){
		*command = nothing;
	}else if(!(strcmp(argv[1],"--load"))){
		*command = load;
	}else if(!(strcmp(argv[1],"--save"))){
		*command = save;
	}else if(!(strcmp(argv[1],"--distances"))){
		*command = distances;
	}else{
		return -2;
	}
	return 0;
}


