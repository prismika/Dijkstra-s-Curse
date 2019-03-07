#include <ncurses.h>
#include <stdio.h>
#include "inputCollector.h"

int inputState_init(InputState * inState){
	raw();
	noecho();
	keypad(stdscr, TRUE);
	inState->lastType = input_null;
	return 0;
}

int inputState_update(InputState * inState){
	int in = getch();
	InputType newType;
	switch(in){
		case '7':
		case 'y':
		newType = input_upleft;
		break;

		case '8':
		case 'k':
		newType = input_up;
		break;

		case '9':
		case 'u':
		newType = input_upright;
		break;

		case '6':
		case 'l':
		newType = input_right;
		break;

		case '3':
		case 'n':
		newType = input_downright;
		break;

		case '2':
		case 'j':
		newType = input_down;
		break;

		case '1':
		case 'b':
		newType = input_downleft;
		break;

		case '4':
		case 'h':
		newType = input_left;
		break;

		case '>':
		newType = input_downstairs;
		break;

		case '<':
		newType = input_upstairs;
		break;

		case '5':
		case ' ':
		case '.':
		newType = input_rest;
		break;

		case 'm':
		newType = input_mlist;
		break;

		case KEY_UP:
		newType = input_mlist_up;
		break;

		case KEY_DOWN:
		newType = input_mlist_down;
		break;

		case KEY_EXIT:
		newType = input_escape;
		break;

		case 'Q':
		newType = input_quit;
		break;

		default:
		newType = input_null;
		break;
	}
	inState->lastType = newType;
	return 0;
}

InputType inputState_get_last(InputState * inState){
	return inState->lastType;
}

