#ifndef INPUTCOLLECTOR_H
#define INPUTCOLLECTOR_H

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

typedef struct{
	InputType lastType;
}InputState;

int inputState_init(InputState * inState);
int inputState_update(InputState * inState);
InputType inputState_get_last(InputState * inState);





#endif