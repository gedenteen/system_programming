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
	initscr(); //начало работы с ncurses
	signal(SIGWINCH, sig_winch); //установить обработку сигнала
	cbreak(); //для getch() - символ считывается без enter'a
	noecho(); //отключить отображение символов, вводимых с клавиаутры
	curs_set(0); //курсор мыши невидим
	attron(A_BOLD); //сделать текст жирным
	move(5, 15);
	if (can_change_color())
		printw("can change color!\n");
	else
		printw("can't change color\n");
	attroff(A_BOLD); //отключить жирный текст
	attron(A_BLINK);
	move(7, 16);
	printw("Press any key to continue...");
	refresh(); //обновить содержимое экрана
	getch();
	endwin(); //конец работы с ncurses
	exit(EXIT_SUCCESS);
}

