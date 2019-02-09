all: dungeonGen

dungeonGen: dungeonGen.c mapElements
	gcc -Wall -ggdb3 dungeonGen.c mapElements.o -o dungeonGen -lm

mapElements: mapElements.h mapElements.c
	gcc -Wall -ggdb3 -o mapElements.o -c mapElements.c

clean:
	rm dungeonGen mapElements.o