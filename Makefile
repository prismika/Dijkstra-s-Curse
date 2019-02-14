all: dungeonGen

dungeonGen: dungeonGen.c mapElements dij
	gcc -Wall -ggdb3 dungeonGen.c mapElements.o -o dungeonGen -lm

mapElements: mapElements.h mapElements.c
	gcc -Wall -ggdb3 -o mapElements.o -c mapElements.c

dij: dij.c dij.h
	gcc -Wall -ggdb -o dij.o -c dij.c

test: test.c mapElements
	gcc -Wall -ggdb3 test.c mapElements.o -o test -lm
	./test
dijtest: dijtest.c mapElements dij
	gcc -Wall -ggdb3 dijtest.c mapElements.o dij.o -o dijtest -lm
clean:
	rm dungeonGen mapElements.o test mapElements.o dijtest.o