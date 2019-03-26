#ifndef GAMETYPES_H
#define GAMETYPES_H

#include "map.h"
#include "mapElements.h"
#include "mapBuilder.h"
#include "pathFinder.h"
#include "display.h"
#include "mapPopulator.h"
#include "turnMaster.h"
#include "inputCollector.h"



class OriginalGameType{
public:
	OriginalGameType(int seed, int nummon);
	int runGame();
private:
	long seed;
	int nummon;
	Map theMap;
	TurnMaster turnMaster;
	InputState inputState;
	enum {
		mode_map,
		mode_monster_list
	}mode;
	int scrollOffset;
	void init_level();
	void delete_level();
	void quit_game();
	int interpret_pc_input(Entity * pc, InputState * inState);
	void handle_death(void);
};



#endif