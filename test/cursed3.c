#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>

void sig_winch(int signo)
{
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
	resizeterm(size.ws_row, size.ws_col);
}

int main(int argc, char ** argv)
{
	WINDOW * wnd;
	WINDOW * subwnd;
	
	initscr();
	signal(SIGWINCH, sig_winch);
	curs_set(FALSE);
	start_color(); //инициализировать управление цветом
	refresh();
	init_pair(1, COLOR_BLUE, COLOR_GREEN); //создание пар цветов: номер пары, foreground, background
	init_pair(2, COLOR_YELLOW, COLOR_BLUE);
	wnd = newwin(5, 18, 2, 4);
	wattron(wnd, COLOR_PAIR(1)); //установить цветовую пару
	box(wnd, '|', '-');
	subwnd = derwin(wnd, 3, 16, 1, 1);
	wbkgd(subwnd, COLOR_PAIR(2)); //как attron, но заполняет массив символов окна?
	wprintw(subwnd, "Hello, brave new curses");
	wattron(subwnd, A_BOLD); //чтобы сделать цвет ярким
	wprintw(subwnd, " world!\n");
	wrefresh(subwnd);
	wrefresh(wnd);
	delwin(subwnd);
	delwin(wnd);
	wmove(stdscr, 8, 1);
	printw("Press any key to continue...");
	refresh();
	getch();
	endwin();
	exit(EXIT_SUCCESS);
}
