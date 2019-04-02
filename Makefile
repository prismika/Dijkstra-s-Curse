CC = gcc
CXX = g++
ECHO = echo
RM = rm
TAR = tar cvfz

ASSIGNMENT = 1.06

CFLAGS = -Wall -ggdb3

BIN = dijkstrasCurse
OBJS = 			mapElements.o 	pathFinder.o	display.o 	heap.o 		mapBuilder.o 	mapIO.o 	map.o 	populationElements.o 	mapPopulator.o 		turnMaster.o 	inputCollector.o 	gameTypes.o 	monsterParser.o
CSUBMISSION = 	mapElements.cpp pathFinder.cpp 	display.cpp	heap.cpp 	mapBuilder.cpp 	mapIO.cpp 	map.cpp	populationElements.cpp 	mapPopulator.cpp 	turnMaster.cpp 	inputCollector.cpp 	gameTypes.cpp 	monsterParser.cpp 	dijkstrasCurse.cpp
HSUBMISSION = 	mapElements.h 	pathFinder.h 	display.h 	heap.h 		mapBuilder.h 	mapIO.h 	map.h 	populationElements.h 	mapPopulator.h 		turnMaster.h 	inputCollector.h 	gameTypes.h 	monsterParser.h
TXTSUBMISSION = README CHANGELOG Makefile
TESTS = test

all: $(BIN)

# dijkstrasCurse: dijkstrasCurse.c mapElements dij display heap
# 	gcc -Wall -ggdb3 dijkstrasCurse.c mapElements.o display.o pathFinder.o heap.o -o dijkstrasCurse -lm
$(BIN): $(OBJS) dijkstrasCurse.o
	@$(ECHO) Linking $@
	@$(CXX) $^ -o $@ -lncurses

-include $(OBJS:.o=.d)

%.o: %.cpp
	@$(ECHO) Compiling $<
	@$(CXX) $(CFLAGS) -MMD -MF $*.d -c $<

#Test things
test: test.o $(OBJS)
	@$(ECHO) Linking $@
	@$(CC) $^ -o $@ -lncurses

	
#Make the tarball

tarball:
	@$(ECHO) Turning your hard work into a literal ball of tar
	mkdir -p Vose_Brantley-assignment-$(ASSIGNMENT)
	cp $(CSUBMISSION) $(HSUBMISSION) $(TXTSUBMISSION)  Vose_Brantley-assignment-$(ASSIGNMENT)
	$(TAR) ../tarballs/Vose_Brantley-assignment-$(ASSIGNMENT).tar.gz Vose_Brantley-assignment-$(ASSIGNMENT)


#Clean up!
clean:
	@$(ECHO) Deleting the junk
	@$(RM) *.o $(BIN) $(TESTS) *.d