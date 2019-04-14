#include <ncurses.h>
#include <stdio.h>
#include "inputCollector.h"

int inputState_init(InputState * inState){
	raw();
	noecho();
	keypad(stdscr, true);
	inState->lastType = input_null;
	inState->isMovement = false;
	inState->isStair = false;
	return 0;
}

int inputState_update(InputState * inState){
	InputType newType = input_null;
	while(newType == input_null){
		int in = getch();
		//Is it a movement?
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

			case '5':
			case ' ':
			case '.':
			newType = input_rest;
			break;

			default:break;
		}
		//Did we find a movement?
		if(newType != input_null){
			inState->isMovement = true;
			inState->isStair = false;
			inState->lastType = newType;
			return 0;
		}

		//Check for stair commands
		switch(in){
			case '>':
			newType = input_downstairs;
			break;

			case '<':
			newType = input_upstairs;
			break;

			default: break;
		}
		//Did we find a stair command?
		if(newType != input_null){
			inState->isStair = true;
			inState->isMovement = false;
			inState->lastType = newType;
			return 0;
		}

		//Check for other command types
		switch(in){
			case 'Q':
			newType = input_quit;
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

			case 'q':
			case 27:
			newType = input_escape;
			break;

			case 'T':
			newType = input_teleport;
			break;

			case 'r':
			newType = input_random;
			break;

			case 'f':
			newType = input_fog;
			break;

			case 'w':
			newType = input_wear;
			break;

			case 't':
			newType = input_t;
			break;
			case 'x':
			newType = input_x;
			break;
			case 'I':
			newType = input_I;
			break;
			case 'L':
			newType = input_L;
			break;

			case '0':
			newType = input_0;
			break;

			case 'a':
			newType = input_a;
			break;
			case 'c':
			newType = input_c;
			break;
			case 'd':
			newType = input_d;
			break;
			case 'e':
			newType = input_e;
			break;
			case 'g':
			newType = input_g;
			break;
			case 'i':
			newType = input_i;
			break;

			default://If we reach this point, the input wasn't recognized
			newType = input_null;
			break;
		}
		inState->isStair = false;
		inState->isMovement = false;
		inState->lastType = newType;
	}
	return 0;
}

InputType inputState_get_last(InputState * inState){
	return inState->lastType;
}

bool inputState_is_movement(InputState * inState){
	return inState -> isMovement;
}

bool inputState_is_stair(InputState * inState){
	return inState -> isStair;
}
