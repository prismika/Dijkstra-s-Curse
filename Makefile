all: dungeonGen

dungeonGen: dungeonGen.c mapElements dij display
	gcc -Wall -ggdb3 dungeonGen.c mapElements.o display.o -o dungeonGen -lm

mapElements: mapElements.h mapElements.c
	gcc -Wall -ggdb3 -o mapElements.o -c mapElements.c

dij: dij.c dij.h display
	gcc -Wall -ggdb3 -o dij.o display.o -c dij.c

display: display.h display.c mapElements
	gcc -Wall -ggdb3 -o display.o mapElements.o -c display.c

#Test things
test: test.c mapElements
	gcc -Wall -ggdb3 test.c mapElements.o -o test -lm
	./test

dijtest: dijtest.c mapElements dij display
	gcc -Wall -ggdb3 dijtest.c mapElements.o dij.o display.o -o dijtest -lm

#Clean up!
clean:
	rm dungeonGen mapElements.o test mapElements.o dijtest dij.o display.o