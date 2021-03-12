#include <termios.h>
#include <sys/ioctl.h> //для обработки сигналов
#include <signal.h> //для соединения сигнала и функции-обработчика
#include <stdlib.h>
#include <curses.h> //ncurses
#include <dirent.h> //для просмотра каталогов и файлов
#include <string.h>
#include <malloc.h>

int Min(int a, int b) {
	if (a < b)
		return a;
	else
		return b;
}

void Handling_SIGWINCH(int signo) { //обработчик сигнала SIGWINCH
	struct winsize size; //структура для размеров окна
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); //получить размеры окна
	resizeterm(size.ws_row, size.ws_col); //изменить размеры окна
}

int Print_files(WINDOW *wnd, struct dirent **namelist, int n, 
	        int upper_bound, int lower_bound, int chosen_row) {
	werase(wnd); //заполнить окно пробелами
	for (int i = upper_bound; i < Min(n, lower_bound); i++) { 
		wmove(wnd, i - upper_bound, 0);
		if (i == chosen_row) {
			wattron(wnd, COLOR_PAIR(1));
			wprintw(wnd, "%s", namelist[i]->d_name);
			wattron (wnd, COLOR_PAIR(2));
		}
		else {
			wprintw(wnd, "%s", namelist[i]->d_name);
		}
	}
	wrefresh(wnd);	        
	return 0;	        
}

int Change_chosen_row(WINDOW *wnd, struct dirent **namelist,
		      int upper_bound, int *chosen_row, int offset) {
	wattron(wnd, COLOR_PAIR(2));
	wmove(wnd, *chosen_row - upper_bound, 0);
	wprintw(wnd, "%s", namelist[*chosen_row]->d_name);

	*chosen_row += offset; //смещение строки

	wattron(wnd, COLOR_PAIR(1));
	wmove(wnd, *chosen_row - upper_bound, 0);
	wprintw(wnd, "%s", namelist[*chosen_row]->d_name);
	wattron (wnd, COLOR_PAIR(2));
	wrefresh(wnd);
}

int main() {
	WINDOW * wnd1, *wnd2;
	WINDOW * subwnd1;
	
	initscr();
	signal(SIGWINCH, Handling_SIGWINCH);
	cbreak();
	curs_set(FALSE); //курсор невидимый
	start_color(); //начать работу с цветом терминала (ncurses)
	
	struct winsize size; //структура для размеров окна
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); //получить размеры окна
	int mid_of_terminal = size.ws_col / 2;
	wnd1 = newwin(size.ws_row, mid_of_terminal, 0, 0); //задать размеры окна
	box(wnd1, '|', '-'); //символы для границ окна
	//под-окно, чтобы не затереть границы окна:
	subwnd1 = derwin(wnd1, size.ws_row - 2, mid_of_terminal - 2, 1, 1); 
	wrefresh(wnd1);
	
	/// второе окно: ///
	wnd2 = newwin(size.ws_row, mid_of_terminal, 0, mid_of_terminal); 
	init_pair(1, COLOR_BLACK, COLOR_YELLOW); //цвет для выделения выбранного файла
	init_pair(2, COLOR_WHITE, COLOR_BLACK); //цвет для остальных файлов
	wbkgd(wnd2, COLOR_PAIR(1) | A_BOLD);
	box(wnd2, '|', '-');
	wrefresh(wnd2);
	wmove(wnd2, 9, 15);
	wprintw(wnd2, "Press any key to continue...");
	wrefresh(wnd2);
	
	struct dirent **namelist;
	int n, chosen_row = 0; //кол-во файлов, выбранная строка (файл)
	int upper_bound = 0, lower_bound = size.ws_row - 2; //границы выводимых файлов
	char* dir_name = (char*) malloc(sizeof(char) * 2); //путь к выбранной директории
	strcpy(dir_name, "."); //изначально выбранная директория = папка, откуда запускается программа
	n = scandir(dir_name, &namelist, 0, alphasort); //получить массив имен файлов (отсортированный)
	if (n < 0)
		perror("scandir");
	else 
		Print_files(subwnd1, namelist, n, upper_bound, lower_bound, chosen_row);
	
	char ch;
	wmove(subwnd1, 0, mid_of_terminal - 5); //переместиться за точку, чтобы не перекрывать
	//надо отключить вывод символов
	while ((ch = wgetch(subwnd1)) != 10) {
		if (ch == 119 && chosen_row > 0) { //'w'
			if (chosen_row == upper_bound && upper_bound > 0) {
				upper_bound--;
				lower_bound--;
				chosen_row--;
				Print_files(subwnd1, namelist, n, upper_bound, lower_bound, chosen_row);
			}
			else {	
				Change_chosen_row(subwnd1, namelist, upper_bound, &chosen_row, -1);
			}
		}
		else if (ch == 115) { //'s'
			//если выделена самая нижняя строка, но ниже есть еще файлы:
			if (chosen_row == lower_bound - 1 && chosen_row < n - 1) {
				upper_bound++;
				lower_bound++;
				chosen_row++;
				Print_files(subwnd1, namelist, n, upper_bound, lower_bound, chosen_row);
			}
			else if (chosen_row < n - 1) { //'s'
				Change_chosen_row(subwnd1, namelist, upper_bound, &chosen_row, 1);
			}
			else
				chosen_row = n - 1;
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
			//временная строка для проверка нового пути:
			char *new_dir_name;
			//ее размер = старый путь + '\' + новая папка + '\0'
			int new_dir_len = strlen(dir_name) + strlen(namelist[chosen_row]->d_name) + 2;
			new_dir_name = (char*) malloc(sizeof(char) * new_dir_len);
			strcpy(new_dir_name, dir_name); //скопировать текущий путь
			strcat(new_dir_name, "/"); //добавить в конец пути новую папку
			strcat(new_dir_name, namelist[chosen_row]->d_name);
			if (opendir(new_dir_name) == NULL) { //если указан файл, а не папка
				free(new_dir_name);
				wmove(subwnd1, 0, 10); 
				wprintw(subwnd1, "this isn't a file");
				wrefresh(subwnd1);
				continue;
			}
			free(new_dir_name); //временная строка больше не нужна
			dir_name = realloc(dir_name, sizeof(char) * new_dir_len);
			strcat(dir_name, "/"); 
			strcat(dir_name, namelist[chosen_row]->d_name); 
			/*wmove(subwnd1, 0, 10); //удалить 
			wprintw(subwnd1, "%s", dir_name); //удалить
			wrefresh(subwnd1);
			if (chosen_row > 5)
				while(1){}*/
			
			chosen_row = 0; //выбранная строка в новой директории - это '.'
			werase(subwnd1); //заполнить окно пробелами
			for (int i = 0; i < n; i++) //очистить память для массива файлов
				free(namelist[i]);
			free(namelist);
			n = scandir(dir_name, &namelist, 0, alphasort); //получить новый массив
			Print_files(subwnd1, namelist, n, upper_bound, lower_bound, chosen_row);
		}
		wmove(subwnd1, 0, mid_of_terminal - 5);
	}
	
	//TODO: баг при открытии CUDA
	//TODO: нужно освобождение памяти namelist и dir_name
	//TODO: обработка ошибок
	delwin(subwnd1); //удалить структуру, но текст останется
	delwin(wnd1);
	delwin(wnd2);
	getch();
	endwin(); //конец работы с ncurses
	return 0;
}
