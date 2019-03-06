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
	mvaddch(1, 1, 'X');

	int i=0;
	while(i<=21){
		char input = getch();
		mvaddch(i++, 1, input);
		refresh();
		usleep(250000);
	}
	endwin();

	printf("\n\n"
"    ___   _    _  _          _                 _      \n"
"   /   \\ (_)  (_)| | __ ___ | |_  _ __   __ _ ( ) ___ \n"
"  / /\\ / | |  | || |/ // __|| __|| '__| / _` ||/ / __|\n"
" / /_//  | |  | ||   < \\__ \\| |_ | |   | (_| |   \\__ \\\n"
"/___ /   |_| _/ ||_|\\_\\|___/ \\__||_|    \\__,_|   |___/\n"
"            |__/ \n"
"                 (      (           \n"
"   (             )\\ )   )\\ )        \n"
"   )\\       (   (()/(  (()/(   (    \n"
" (((_)      )\\   /(_))  /(_))  )\\   \n"
" )\\___   _ ((_) (_))   (_))   ((_)  \n"
"((/ __| | | | | | _ \\  / __|  | __| \n"
" | (__  | |_| | |   /  \\__ \\  | _|  \n"
"  \\___|  \\___/  |_|_\\  |___/  |___| \n"
"\n\n"
		);
}

