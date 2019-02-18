CC = gcc
CXX = g++
ECHO = echo
RM = rm -f
TAR = tar cvfz

ASSIGNMENT = 1.03

CFLAGS = -Wall -ggdb3

BIN = dungeonGen
OBJS = dungeonGen.o mapElements.o dij.o display.o heap.o
CSUBMISSION = mapElements.c dij.c display.c heap.c dungeonGen.c 
HSUBMISSION = mapElements.h dij.h display.h heap.h
TXTSUBMISSION = README CHANGELOG Makefile
TESTS = test dijtest

all: $(BIN)

# dungeonGen: dungeonGen.c mapElements dij display heap
# 	gcc -Wall -ggdb3 dungeonGen.c mapElements.o display.o dij.o heap.o -o dungeonGen -lm
$(BIN): $(OBJS)
	@$(ECHO) Linking $@
	@$(CC) $^ -o $@

-include $(OBJS:.o=.d)

%.o: %.c
	@$(ECHO) Compiling $<
	@$(CC) $(CFLAGS) -MMD -MF $*.d -c $<

# mapElements: mapElements.h mapElements.c
# 	gcc -Wall -ggdb3 -o mapElements.o -c mapElements.c

# dij: dij.c dij.h heap
# 	gcc -Wall -ggdb3 -o dij.o -c dij.c

# display: display.h display.c mapElements
# 	gcc -Wall -ggdb3 -o display.o -c display.c

# heap: heap.c heap.h
# 	gcc -Wall -ggdb3 -c heap.c -o heap.o

#Test things
test: test.c heap.o
	@$(ECHO) Linking $@
	@$(CC) $^ -o $@

dijtest: dijtest.c mapElements.o dij.o display.o heap.o
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