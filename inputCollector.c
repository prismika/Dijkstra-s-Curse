#include <ncurses.h>
#include <stdio.h>
#include "inputCollector.h"

int inputState_init(InputState * inState){
	raw();
	noecho();
	keypad(stdscr, TRUE);
	inState->lastType = noInput;
	return 0;
}

int inputState_update(InputState * inState){
	char in = getch();
	switch(in){
		case 'k':
		inState->lastType = up;
		break;
		default:
		inState->lastType = noInput;
		break;
	}
	return 0;
}

InputType inputState_get_last(InputState * inState){
	return inState->lastType;
}

