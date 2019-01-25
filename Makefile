all: dungeonGen

dungeonGen: dungeonGen.c
	gcc -Wall -ggdb3 dungeonGen.c -o dungeonGen

clean:
	rm dungeonGen
	rm dungeonGen.exe