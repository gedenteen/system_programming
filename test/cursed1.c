#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h> //ncurses

void sig_winch(int signo) //обработчик сигнала SIGWINCH
{
	//структура для размеромов окна:
	struct winsize size;
	//получить размеры окна:
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); 
	//изменить размеры окна:
	resizeterm(size.ws_row, size.ws_col); 
}

int main(int argc, char ** argv)
{
	WINDOW * wnd;
	WINDOW * subwnd;
	
	initscr();
	signal(SIGWINCH, sig_winch);
	cbreak();
	curs_set(0); //курсор невидимый
	refresh();
	wnd = newwin(6, 18, 0, 24); //сначала размеры окна, потом координата верхнего левого угла в stdscr
	box(wnd, '|', '-');
	subwnd = derwin(wnd, 4, 16, 1, 1); //под-окно, чтобы не затереть границы окна
	wprintw(subwnd, "Hello, brave new curses world!\n");
	wrefresh(wnd);
	delwin(subwnd); //удалить структуру, но текст останется
	delwin(wnd);
	move(9, 0);
	printw("Press any key to continue...");
	refresh();
	getch();
	endwin();
	exit(EXIT_SUCCESS);
}
