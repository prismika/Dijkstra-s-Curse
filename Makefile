CC = gcc
CXX = g++
ECHO = echo
RM = rm
TAR = tar cvfz

ASSIGNMENT = 1.04

CFLAGS = -Wall -ggdb3

BIN = dijkstrasCurse
OBJS = 			mapElements.o pathFinder.o display.o heap.o mapBuilder.o mapIO.o map.o populationElements.o mapPopulator.o turnMaster.o dijkstrasCurse.o
CSUBMISSION = 	mapElements.c pathFinder.c display.c heap.c mapBuilder.c mapIO.c map.c populationElements.c mapPopulator.c turnMaster.c dijkstrasCurse.c
HSUBMISSION = 	mapElements.h pathFinder.h display.h heap.h mapBuilder.h mapIO.h map.h populationElements.h mapPopulator.h turnMaster.h
TXTSUBMISSION = README CHANGELOG Makefile
TESTS = test

all: $(BIN)

# dijkstrasCurse: dijkstrasCurse.c mapElements dij display heap
# 	gcc -Wall -ggdb3 dijkstrasCurse.c mapElements.o display.o pathFinder.o heap.o -o dijkstrasCurse -lm
$(BIN): $(OBJS)
	@$(ECHO) Linking $@
	@$(CC) $^ -o $@ -lncurses

-include $(OBJS:.o=.d)

%.o: %.c
	@$(ECHO) Compiling $<
	@$(CC) $(CFLAGS) -MMD -MF $*.d -c $<

#Test things
test: test.o
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