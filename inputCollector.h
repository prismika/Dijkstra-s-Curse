#ifndef INPUTCOLLECTOR_H
#define INPUTCOLLECTOR_H

typedef enum{
up
}InputType;

typedef struct{
InputType lastType;

}InputState;

int inputState_init(InputState inState);





#endif