#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>


int main(int argc, char * argv[]){
	initscr();
	raw();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);

	int i=0;
	while(i<=21){
		int input = getch();
		mvprintw(i++, 1, "%d", input);
		usleep(25000);
	}
	endwin();
}

