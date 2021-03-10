#include <termios.h>
#include <sys/ioctl.h> //для обработки сигналов
#include <signal.h> //для соединения сигнала и функции-обработчика
#include <stdlib.h>
#include <curses.h> //ncurses
#include <dirent.h> //для просмотра каталогов и файлов
#include <string.h>

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
	
	/// второе окно: ///
	wnd2 = newwin(20, 30, 0, 30);
	init_pair(1, COLOR_BLACK, COLOR_YELLOW); 
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	wbkgd(wnd2, COLOR_PAIR(1) | A_BOLD);
	box(wnd2, '|', '-');
	wrefresh(wnd2);
	wmove(wnd2, 9, 15);
	wprintw(wnd2, "Press any key to continue...");
	wrefresh(wnd2);
	
	struct dirent **namelist;
	int n, chosen_row = 0;
	char* dir_name = (char*) malloc(sizeof(char) * 2);
	strcpy(dir_name, ".");
	n = scandir(dir_name, &namelist, 0, alphasort);
	if (n < 0)
		perror("scandir");
	else {
		for (int i = 0; i < n; i++) { 
			if (i == chosen_row)
				wattron(subwnd1, COLOR_PAIR(1));
			else
				wattron (subwnd1, COLOR_PAIR(2));
			wmove(subwnd1, i, 0);
			wprintw(subwnd1, "%s", namelist[i]->d_name);
			wrefresh(subwnd1);
		}
	}
	
	char ch;
	wmove(subwnd1, 0, 1); //переместиться за точку, чтобы ничего не ломать
	//надо отключить вывод символов
	while ((ch = wgetch(subwnd1)) != 10) {
		if (ch == 119 && chosen_row > 0) { //'w'
			wattron(subwnd1, COLOR_PAIR(2));
			wmove(subwnd1, chosen_row, 0);
			wprintw(subwnd1, "%s", namelist[chosen_row]->d_name);
			chosen_row--;
		}
		else if (ch == 115 && chosen_row < n - 1) { //'s'
			wattron(subwnd1, COLOR_PAIR(2));
			wmove(subwnd1, chosen_row, 0);
			wprintw(subwnd1, "%s", namelist[chosen_row]->d_name);
			chosen_row++;
		}
		else if (ch == 113) { //'q'
			delwin(subwnd1); //удалить структуру, но текст останется
			delwin(wnd1);
			delwin(wnd2);
			//getch();
			endwin(); //конец работы с ncurses
			exit(EXIT_SUCCESS);
		}
		else if (ch == 101) { //'e'
			//char *new_dir;
			//добавить имя папки к сохраненному пути:
			strcat(dir_name, "/");
			strcat(dir_name, namelist[chosen_row]->d_name); 
			if (opendir(dir_name) == NULL) { //
				wmove(subwnd1, 0, 10); 
				wprintw(subwnd1, "this isn't a file");
				wrefresh(subwnd1);
				continue;
			}
			
			chosen_row = 0;
			werase(subwnd1); //заполнить окно пробелами
			for (int i = 0; i < n; i++) {
				free(namelist[i]);
			}
			free(namelist);
			n = scandir(dir_name, &namelist, 0, alphasort);
			
			wmove(subwnd1, 0, 5); wprintw(subwnd1, "n=%d", n); //
			for (int i = 0; i < n; i++) { 
				if (i == chosen_row)
					wattron(subwnd1, COLOR_PAIR(1));
				else
					wattron (subwnd1, COLOR_PAIR(2));
				wmove(subwnd1, i, 0);
				wprintw(subwnd1, "%s", namelist[i]->d_name);
				wmove(subwnd1, 0, 10); wprintw(subwnd1, "i=%d", i); //
				wrefresh(subwnd1);
			}
			wmove(subwnd1, 0, 15); wprintw(subwnd1, "ok"); //
		}
		wattron(subwnd1, COLOR_PAIR(1));
		wmove(subwnd1, chosen_row, 0);
		wprintw(subwnd1, "%s", namelist[chosen_row]->d_name);
		wattron (subwnd1, COLOR_PAIR(2));
		wrefresh(subwnd1);
		
		wmove(subwnd1, 0, 1);
	}
	
	//TODO: нужно освобождение памяти namelist
	delwin(subwnd1); //удалить структуру, но текст останется
	delwin(wnd1);
	delwin(wnd2);
	getch();
	endwin(); //конец работы с ncurses
	return 0;
}
