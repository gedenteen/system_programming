#include <termios.h>
#include <sys/ioctl.h> //для обработки сигналов
#include <signal.h> //для соединения сигнала и функции-обработчика
#include <stdlib.h>
#include <curses.h> //ncurses
#include <dirent.h> //для просмотра каталогов и файлов
#include <string.h>
#include <malloc.h>

int Min(int a, int b) 
{
	if (a < b)
		return a;
	else
		return b;
}

void Handling_SIGWINCH(int signo) 
{ //обработчик сигнала SIGWINCH
	struct winsize size; //структура для размеров окна
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); //получить размеры окна
	resizeterm(size.ws_row, size.ws_col); //изменить размеры окна
}

int Print_files(WINDOW *wnd, struct dirent **namelist, int n, 
	int upper_bound, int lower_bound, int chosen_row) 
{
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
		      int upper_bound, int *chosen_row, int offset) 
{
	wattron(wnd, COLOR_PAIR(2)); //убрать "подсвечивание" старой выбранной строки
	wmove(wnd, *chosen_row - upper_bound, 0);
	wprintw(wnd, "%s", namelist[*chosen_row]->d_name);
	*chosen_row += offset; //смещение строки
	wattron(wnd, COLOR_PAIR(1)); //добавить "подсвечивание" новой выбранной строки
	wmove(wnd, *chosen_row - upper_bound, 0);
	wprintw(wnd, "%s", namelist[*chosen_row]->d_name);
	wattron (wnd, COLOR_PAIR(2));
	wrefresh(wnd);
	return 0;
}

int Key_up(WINDOW *wnd, struct dirent **namelist, int n,
	int *chosen_row, int *upper_bound, int *lower_bound) 
{ //функция для нажатия клавиши вверх ('w')
	//если выбранная строка самая верхняя, но выше есть еще файлы:
	if (*chosen_row == *upper_bound && *upper_bound > 0) {
		(*upper_bound)--; //тогда уменьшить границы на 1
		(*lower_bound)--;
		(*chosen_row)--; //выбранная строка тоже на -1
		Print_files(wnd, namelist, n, *upper_bound, *lower_bound, *chosen_row);
	}
	else if (*chosen_row > 0) { //иначе изменить только выбранную строку
		Change_chosen_row(wnd, namelist, *upper_bound, chosen_row, -1);
	} //иначе chosen_row остается равна 0
	return 0;
}

int Key_down(WINDOW *wnd, struct dirent **namelist, int n,
	int *chosen_row, int *upper_bound, int *lower_bound) 
{
	//если выделена самая нижняя строка, но ниже есть еще файлы:
	if (*chosen_row == *lower_bound - 1 && *chosen_row < n - 1) {
		(*upper_bound)++;
		(*lower_bound)++;
		(*chosen_row)++;
		Print_files(wnd, namelist, n, *upper_bound, *lower_bound, *chosen_row);
	}
	else if (*chosen_row < n - 1) { //'s'
		Change_chosen_row(wnd, namelist, *upper_bound, chosen_row, 1);
	} //иначе chosen_row остается равна n - 1
	return 0;
}

int main() {
	initscr();
	signal(SIGWINCH, Handling_SIGWINCH);
	cbreak();
	curs_set(FALSE); //курсор невидимый
	
	struct winsize size; //структура для размеров окна
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); //получить размеры окна
	int mid_of_terminal = size.ws_col / 2;
	
	//--------- СОЗДАНИЕ ОКОН  ---------//
	WINDOW *wnd1, *wnd2;
	WINDOW *subwnd1, *subwnd2;
	//первое окно:
	wnd1 = newwin(size.ws_row, mid_of_terminal, 0, 0); //задать размеры окна
	box(wnd1, '|', '-'); //символы для границ окна
	//под-окно, чтобы не затереть границы окна:
	subwnd1 = derwin(wnd1, size.ws_row - 2, mid_of_terminal - 2, 1, 1); 
	if (subwnd1 == NULL) {
		endwin(); //конец работы с ncurses
		printf("error: can't create subwnd1\n");
		exit(1);
	}
	wrefresh(wnd1);
	//второе окно:
	wnd2 = newwin(size.ws_row, mid_of_terminal, 0, mid_of_terminal); 
	if (wnd2 == NULL) {
		endwin(); //конец работы с ncurses
		printf("error: can't create wnd2\n");
		exit(1);
	}
	box(wnd2, '|', '-');
	subwnd2 = derwin(wnd2, size.ws_row - 2, mid_of_terminal - 2, 1, 1); 
	if (subwnd2 == NULL) {
		endwin(); //конец работы с ncurses
		printf("error: can't create subwnd2\n");
		exit(1);
	}
	wrefresh(wnd2);
	
	//--------- ЦВЕТ ---------//
	start_color(); //начать работу с цветом терминала (ncurses)
	init_pair(1, COLOR_BLACK, COLOR_YELLOW); //цвет для выделения выбранного файла
	init_pair(2, COLOR_WHITE, COLOR_BLACK); //цвет для остальных файлов

	//--------- ФАЙЛЫ ДЛЯ ЛЕВОГО ОКНА ---------//
	struct dirent **namelist;
	int n, chosen_row = 0; //всего файлов, выбранная строка (файл)
	int upper_bound = 0, lower_bound = size.ws_row - 2; //границы выводимых файлов
	char* dir_name = (char*) malloc(sizeof(char) * 2); //путь к выбранной директории
	strcpy(dir_name, "."); //изначально выбранная директория = папка, откуда запускается программа
	n = scandir(dir_name, &namelist, 0, alphasort); //получить массив имен файлов (отсортированный)
	if (n < 0)
		perror("scandir");
	else 
		Print_files(subwnd1, namelist, n, upper_bound, lower_bound, chosen_row);
		
	//--------- ФАЙЛЫ ДЛЯ ПРАВОГО ОКНА ---------//
	struct dirent **namelist2;
	int n2, chosen_row2 = 0; //всего файлов, выбранная строка (файл)
	int upper_bound2 = 0, lower_bound2 = size.ws_row - 2; //границы выводимых файлов	
	char* dir_name2 = (char*) malloc(sizeof(char) * 2); //путь к выбранной директории
	strcpy(dir_name2, "."); //изначально выбранная директория = папка, откуда запускается программа
	n2 = scandir(dir_name2, &namelist2, 0, alphasort); //получить массив имен файлов (отсортированный)
	if (n < 0)
		perror("scandir");
	else 
		Print_files(subwnd2, namelist2, n2, upper_bound2, lower_bound2, chosen_row2);
	
	int current_window = 1;
	char ch;
	wmove(subwnd1, 0, mid_of_terminal - 5); //переместиться за точку, чтобы не перекрывать
	while ((ch = wgetch(subwnd1)) != 10) {
		if (ch == 'a') { //клавиша влево - переключиться на 1-ое окно
			current_window = 1;
		}
		else if (ch == 'd') { //клавиша вправо - переключиться на 2-ое окно
			current_window = 2;
		}
		else if (ch == 'w') { //клавиша вверх - переключиться на файл выше
			/*if (chosen_row == upper_bound && upper_bound > 0) {
				upper_bound--;
				lower_bound--;
				chosen_row--;
				Print_files(subwnd1, namelist, n, upper_bound, lower_bound, chosen_row);
			}
			else {	
				Change_chosen_row(subwnd1, namelist, upper_bound, &chosen_row, -1);
			}*/
			if (current_window == 1)
				Key_up(subwnd1, namelist, n, &chosen_row, &upper_bound, &lower_bound);
			else
				Key_up(subwnd2, namelist2, n2, &chosen_row2, &upper_bound2, &lower_bound2);
		}
		else if (ch == 115) { //'s'
			/*
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
				chosen_row = n - 1;*/
			if (current_window == 1) 
				Key_down(subwnd1, namelist, n, &chosen_row, &upper_bound, &lower_bound);
			else
				Key_down(subwnd2, namelist2, n2, &chosen_row2, &upper_bound2, &lower_bound2);
		}
		else if (ch == 113) { //'q'
			for (int i = 0; i < n; i++) //очистить память для массива файлов
				free(namelist[i]);
			free(namelist);
			free(dir_name); //очистить строку с путем по папкам
			delwin(subwnd1); //удалить структуры окон
			delwin(wnd1);
			delwin(wnd2);
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
				wprintw(subwnd1, "this isn't a folder");
				wrefresh(subwnd1);
				wmove(subwnd1, 0, mid_of_terminal - 5);
				continue;
			}
			free(new_dir_name); //временная строка больше не нужна
			//проделать все то же самое с основной строкой:
			dir_name = realloc(dir_name, sizeof(char) * new_dir_len);
			strcat(dir_name, "/"); 
			strcat(dir_name, namelist[chosen_row]->d_name); 
			//изменить данные:
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
	//TODO: обработка ошибок
	//TODO: мб надо отключить вывод символов
	/*
	for (int i = 0; i < n; i++) //очистить память для массива файлов
		free(namelist[i]);
	free(namelist);
	free(dir_name);
	delwin(subwnd1); //удалить структуру, но текст останется
	delwin(wnd1);
	delwin(wnd2);
	getch();
	endwin(); //конец работы с ncurses
	*/
	return -1;
}
