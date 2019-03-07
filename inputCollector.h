#ifndef INPUTCOLLECTOR_H
#define INPUTCOLLECTOR_H

typedef enum{
	noInput, up
}InputType;

typedef struct{
	InputType lastType;
}InputState;

int inputState_init(InputState * inState);
int inputState_update(InputState * inState);
InputType inputState_get_last(InputState * inState);





#endif