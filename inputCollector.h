#ifndef INPUTCOLLECTOR_H
#define INPUTCOLLECTOR_H
#include <stdbool.h>

typedef enum{
	input_upleft,		input_up, 		input_upright,
	input_left,			input_null,		input_right,
	input_downleft,		input_down,		input_downright,
	input_downstairs,
	input_upstairs,
	input_rest,
	input_mlist,
	input_mlist_up,
	input_mlist_down,
	input_escape,
	input_quit
}InputType;

class InputState{
public:
	InputType lastType;
	bool isMovement;
	bool isStair;
};

int inputState_init(InputState * inState);
int inputState_update(InputState * inState);
InputType inputState_get_last(InputState * inState);
bool inputState_is_movement(InputState * inState);
bool inputState_is_stair(InputState * inState);





#endif