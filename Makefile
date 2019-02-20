CC = gcc
CXX = g++
ECHO = echo
RM = rm
TAR = tar cvfz

ASSIGNMENT = 1.03

CFLAGS = -Wall -ggdb3

BIN = dijkstrasCurse
OBJS = dijkstrasCurse.o mapElements.o pathFinder.o display.o heap.o mapBuilder.o mapIO.o
CSUBMISSION = mapElements.c pathFinder.c display.c heap.c mapBuilder.c mapio.c dijkstrasCurse.c
HSUBMISSION = mapElements.h pathFinder.h display.h heap.h mapBuilder.h mapio.h
TXTSUBMISSION = README CHANGELOG Makefile
TESTS = test

all: $(BIN)

# dijkstrasCurse: dijkstrasCurse.c mapElements dij display heap
# 	gcc -Wall -ggdb3 dijkstrasCurse.c mapElements.o display.o pathFinder.o heap.o -o dijkstrasCurse -lm
$(BIN): $(OBJS)
	@$(ECHO) Linking $@
	@$(CC) $^ -o $@

-include $(OBJS:.o=.d)

%.o: %.c
	@$(ECHO) Compiling $<
	@$(CC) $(CFLAGS) -MMD -MF $*.d -c $<

# mapElements: mapElements.h mapElements.c
# 	gcc -Wall -ggdb3 -o mapElements.o -c mapElements.c

# dij: pathFinder.c pathFinder.h heap
# 	gcc -Wall -ggdb3 -o pathFinder.o -c pathFinder.c

# display: display.h display.c mapElements
# 	gcc -Wall -ggdb3 -o display.o -c display.c

# heap: heap.c heap.h
# 	gcc -Wall -ggdb3 -c heap.c -o heap.o

#Test things
test: test.c heap.o
	@$(ECHO) Linking $@
	@$(CC) $^ -o $@

	
#Make the tarball

tarball: clean
	@$(ECHO) Turning your hard work into a literal ball of tar
	@$(RM) ../tarballs/Vose_Brantley-assignment-$(ASSIGNMENT).tar.gz
	mkdir -p Vose_Brantley-assignment-$(ASSIGNMENT)
	cp $(CSUBMISSION) $(HSUBMISSION) $(TXTSUBMISSION)  Vose_Brantley-assignment-$(ASSIGNMENT)
	$(TAR) ../tarballs/Vose_Brantley-assignment-$(ASSIGNMENT).tar.gz Vose_Brantley-assignment-$(ASSIGNMENT)
	rm -r Vose_Brantley-assignment-$(ASSIGNMENT)

#Clean up!
clean:
	@$(ECHO) Deleting the junk
	@$(RM) *.o $(BIN) $(TESTS) *.d