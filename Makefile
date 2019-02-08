all: dungeonGen mapFile

dungeonGen: dungeonGen.c
	gcc -Wall -ggdb3 dungeonGen.c -o dungeonGen -lm

mapFile: mapFile.c
	gcc -Wall -ggdb3 mapFile.c -o mapFile -lm

clean:
	rm dungeonGen