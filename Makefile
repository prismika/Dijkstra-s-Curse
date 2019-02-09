all: dungeonGen mapFile

dungeonGen: dungeonGen.c
	gcc -Wall -ggdb3 dungeonGen.c -o dungeonGen -lm

clean:
	rm dungeonGen