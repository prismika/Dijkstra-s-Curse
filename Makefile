all: dungeonGen

dungeonGen: dungeonGen.c mapElements
	gcc -Wall -ggdb3 dungeonGen.c mapElements.o -o dungeonGen -lm

mapElements: mapElements.h mapElements.c
	gcc -Wall -ggdb3 -o mapElements.o -c mapElements.c

test: test.c mapElements
	gcc -Wall -ggdb3 test.c mapElements.o -o test -lm
	./test

clean:
	rm dungeonGen mapElements.o