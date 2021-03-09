#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>

#define MAX_LEN 15
char RIGHT_PASSWORD[3] = {'q', 'w', 'e'};

void sig_winch(int signo)
{
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
	resizeterm(size.ws_row, size.ws_col);
}

void get_password(WINDOW * win, char * password, int max_len)
{
	int i = 0;
	int ch;
	while (((ch = wgetch(win)) != 10) && (i < max_len-1)) {
		if (ch == KEY_BACKSPACE) {
			int x, y;
			if (i==0) continue;
			getyx(win, y, x);
			mvwaddch(win, y, x-1, ' ');
			wrefresh(win);
			wmove(win, y, x-1);
			i--;
			continue;
		}
		//int x, y;
		//if (i==0) continue;
		//getyx(win, y, x);
		//mvwaddch(win, y, x-1, '*');
		password[i++] = ch;
		wechochar(win, '*'); //вывести символ + refresh()
	}
	password[i] = 0;
	wechochar(win, '\n');
}

int main(int argc, char ** argv)
{
	WINDOW * wnd;
	char password[MAX_LEN + 1];
	initscr();
	signal(SIGWINCH, sig_winch);
	curs_set(TRUE);
	start_color();
	refresh();
	init_pair(1, COLOR_YELLOW, COLOR_BLUE);
	wnd = newwin(5, 23, 2, 2);
	wbkgd(wnd, COLOR_PAIR(1));
	
	keypad(wnd, TRUE);
	wprintw(wnd, "Enter password...\n");
	get_password(wnd, password, MAX_LEN);
	wattron(wnd, A_BLINK);
	if (strcmp(password, RIGHT_PASSWORD) == 0)
		wprintw(wnd, "ACCESS GRANTED!");
	else
		wprintw(wnd, "ACCESS DENIED!");
	
	/*
	wattron(wnd, A_BOLD);
	wprintw(wnd, "Enter your name...\n");
	wgetnstr(wnd, name, MAX_NAME_LEN); //считать строку
	name[MAX_NAME_LEN] = 0;
	wprintw(wnd, "Hello, %s!", name);*/
	wrefresh(wnd);
	delwin(wnd);
	curs_set(FALSE);
	move(8, 4);
	printw("Press any key to continue...");
	refresh();
	getch();
	endwin();
	exit(EXIT_SUCCESS);
}
