#include <termios.h>
#include <sys/ioctl.h> //для обработки сигналов
#include <signal.h> //для соединения сигнала и функции-обработчика
#include <stdlib.h>
#include <curses.h> //ncurses
#include <dirent.h> //для просмотра каталогов и файлов

void handling_SIGWINCH(int signo) { //обработчик сигнала SIGWINCH
	struct winsize size; //структура для размеров окна
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); //получить размеры окна
	resizeterm(size.ws_row, size.ws_col); //изменить размеры окна
}

int main() {
	WINDOW * wnd1, *wnd2;
	WINDOW * subwnd1;
	
	initscr();
	signal(SIGWINCH, handling_SIGWINCH);
	cbreak();
	curs_set(0); //курсор невидимый
	start_color();
	refresh();
	wnd1 = newwin(20, 30, 0, 0); //сначала размеры окна, потом координата верхнего левого угла в stdscr
	box(wnd1, '|', '-');
	subwnd1 = derwin(wnd1, 18, 28, 1, 1); //под-окно, чтобы не затереть границы окна
	//wprintw(subwnd1, "Hello, brave new curses world!\n");
	wrefresh(wnd1);
	
	wnd2 = newwin(20, 30, 0, 30);
	init_pair(1, COLOR_BLUE, COLOR_YELLOW);
	wbkgd(wnd2, COLOR_PAIR(1) | A_BOLD);
	box(wnd2, '|', '-');
	wrefresh(wnd2);
	wmove(wnd2, 9, 15);
	wprintw(wnd2, "Press any key to continue...");
	wrefresh(wnd2);
	
	struct dirent **namelist;
	int n;
	n = scandir(".", &namelist, 0, alphasort);
	if (n < 0)
		perror("scandir");
	else {
		//for (int i = 0; i < n; i++) {
		int i = 0;
		while(n--) {
			wmove(subwnd1, i++, 0); 
			wprintw(subwnd1, "%s", namelist[n]->d_name);
			wrefresh(subwnd1);
			free(namelist[n]);
		}	
		free(namelist);
	}
	
	
	delwin(subwnd1); //удалить структуру, но текст останется
	delwin(wnd1);
	delwin(wnd2);
	getch();
	endwin(); //конец работы с ncurses
	return 0;
}
