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

class OriginalGameType;

/*Used for stratey pattern, representing mode of the game*/
class GameMode{
public:
	virtual int execute_mode_actions(OriginalGameType * game) = 0;
	//interpret input
	//Choose what to display
};



/*This is the default mode. Time advances and monsters move and stuff*/
class MovementGameMode : public GameMode{
public:
	int execute_mode_actions(OriginalGameType * game);
private:
	Entity * nextTurnEnt;
	int interpret_pc_input(Entity * pc, InputState * inState, OriginalGameType * game);
};


/*This is the mode in which the monster list is displayed*/
class ListGameMode : public GameMode{
public:
	ListGameMode();
	int execute_mode_actions(OriginalGameType * game);
private:
	int scrollOffset;
	int interpret_pc_input(InputState * inState, OriginalGameType * game);
};

class TeleportGameMode : public GameMode{
public:
	TeleportGameMode();
	int execute_mode_actions(OriginalGameType * game);
private:
	Coordinate cursorPos;
	int interpret_pc_input(InputState * inState, OriginalGameType * game);
};




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
	GameMode * gameMode;
	void init_level();
	void delete_level();
	void quit_game();
	int interpret_pc_input(Entity * pc, InputState * inState);
	void handle_death(void);

	friend class MovementGameMode;
	friend class ListGameMode;
	friend class TeleportGameMode;
};



#endif