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
#include "populationElements.h"

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
	MovementGameMode();
	int execute_mode_actions(OriginalGameType * game);
private:
	bool fog;
	int interpret_pc_input(PC * pc, InputState * inState, OriginalGameType * game);
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
	TeleportGameMode(Coordinate pcPos);
	int execute_mode_actions(OriginalGameType * game);
private:
	Coordinate cursorPos;
	int interpret_pc_input(InputState * inState, OriginalGameType * game);
};

class InventoryWearGameMode : public GameMode{
public:
	InventoryWearGameMode();
	int execute_mode_actions(OriginalGameType * game);
private:
	int interpret_pc_input(InputState * inState, OriginalGameType * game);
};

class InventoryDisplayGameMode : public GameMode{
public:
	InventoryDisplayGameMode();
	int execute_mode_actions(OriginalGameType * game);
private:
	int interpret_pc_input(InputState * inState, OriginalGameType * game);
};

class EquipmentDisplayGameMode : public GameMode{
public:
	EquipmentDisplayGameMode();
	int execute_mode_actions(OriginalGameType * game);
private:
	int interpret_pc_input(InputState * inState, OriginalGameType * game);
};

class EquipmentRemoveGameMode : public GameMode{
public:
	EquipmentRemoveGameMode();
	int execute_mode_actions(OriginalGameType * game);
private:
	int interpret_pc_input(InputState * inState, OriginalGameType * game);
};

class InventoryDestroyGameMode : public GameMode{
public:
	InventoryDestroyGameMode();
	int execute_mode_actions(OriginalGameType * game);
private:
	int interpret_pc_input(InputState * inState, OriginalGameType * game);
};

class InventoryDropGameMode : public GameMode{
public:
	InventoryDropGameMode();
	int execute_mode_actions(OriginalGameType * game);
private:
	int interpret_pc_input(InputState * inState, OriginalGameType * game);
};

class InventoryInspectGameMode : public GameMode{
public:
	InventoryInspectGameMode();
	int execute_mode_actions(OriginalGameType * game);
private:
	int interpret_pc_input(InputState * inState, OriginalGameType * game);
};

class EntityInspectGameMode : public GameMode{
public:
	EntityInspectGameMode(Coordinate pcPos);
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
	PC * pc;
	Map theMap;
	TurnMaster turnMaster;
	InputState inputState;
	MapPopulator mapPopulator;
	GameMode * gameMode;
	void init_level();
	void delete_level();
	void quit_game(bool win);
	int interpret_pc_input(PC * pc, InputState * inState);
	void handle_death(void);
	void title_screen(void);

	friend class MovementGameMode;
	friend class ListGameMode;
	friend class TeleportGameMode;
	friend class InventoryWearGameMode;
	friend class InventoryDisplayGameMode;
	friend class EquipmentDisplayGameMode;
	friend class EquipmentRemoveGameMode;
	friend class InventoryDestroyGameMode;
	friend class InventoryDropGameMode;
	friend class InventoryInspectGameMode;
	friend class EntityInspectGameMode;
};



#endif