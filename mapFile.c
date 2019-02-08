#include <stdio.h>

char *PROGRAM_NAME = "mapFile";

void usage(void);
int parseCL(int argc, char *argv[]);

int main(int argc, char *argv[]){
	parseCL(argc,argv);
	printf("hello brantles");
	return 0;
}

int parseCL(int argc, char *argv[]){
	if(argc<3){
		usage();
		return -1;
	}
	return 0;
}

void usage(void){
	fprintf(stderr, "Usage: %s <--save,--load> <filePath>\n", PROGRAM_NAME);
}