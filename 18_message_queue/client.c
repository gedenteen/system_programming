#include "header.h"

#include <termios.h>
#include <sys/ioctl.h> //для обработки сигналов
#include <signal.h> //для соединения сигнала и функции-обработчика
#include <curses.h> //ncurses
#include <pthread.h>

//TODO посмотреть какие библиотеки для чего нужны

#define RIGHT_WND_WIDTH 25
#define BOTTOM_WND_HEIGHT 5

void Handling_SIGWINCH(int signo) 
{ //обработчик сигнала SIGWINCH
	struct winsize size; //структура для размеров окна
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); //получить размеры окна
	resizeterm(size.ws_row, size.ws_col); //изменить размеры окна
}

int CreateWindow(WINDOW **wnd, WINDOW **subwnd, int height, int width, int x, int y, char* text) 
{
	*wnd = newwin(height, width, x, y); //создать окно(высота, ширина, координаты левого верхнего угла)
	if (*wnd == NULL)
		return 1;
	//box(*wnd, '|', '-'); //символы для границ окна
	wborder(*wnd, '|', '|', '=', '=', '|', '|', '|', '|');
	/// добавление надписи:
	wmove(*wnd, 0, 4);
	wattron(*wnd, COLOR_PAIR(2));
	wprintw(*wnd, text);
	wattron(*wnd, COLOR_PAIR(1));
	wmove(*wnd, 1, 1);
	
	*subwnd = derwin(*wnd, height - 2, width - 2, 1, 1); //под-окно, чтобы не стирать границы окна
	if (*subwnd == NULL)
		return 1;
	wrefresh(*wnd);
	return 0;
}

struct ParamsForThread
{
	mqd_t mqClient;
	WINDOW *subwndChat;
	WINDOW *subwndInput;
	WINDOW *subwndUsers;
};

void *FuncForThread(void *param) 
{
	///
	struct ParamsForThread *pft = (struct ParamsForThread*) param; 
	mqd_t mqClient = pft->mqClient;
	WINDOW *subwndChat = pft->subwndChat;
	WINDOW *subwndInput = pft->subwndInput;
	WINDOW *subwndUsers = pft->subwndUsers;

	int rowsInChat = 0;
	while (1) {
		/// получить сообщение от сервера: 
		char inBuffer[MAX_MSG_SIZE];
		if (mq_receive(mqClient, inBuffer, MSG_BUFFER_SIZE, NULL) == -1) {
		    perror ("Client: mq_receive");
		    exit (1);
		}
		
		/// разбить полученное сообщение на 2 подстроки, которые всегда должны разделяться ';'
		long bytesForCopy = strcspn(inBuffer, ";"); //узнать номер символа ';'
		char substr1[bytesForCopy + 1];//создать переменную для подстроки 1
		strncpy(substr1, inBuffer, bytesForCopy); //скопировать содержимое подстроки 1
		//printf("substr1 = %s\n", substr1);
	   
		/// обработка сообщения "users" - нужно вывести имена (mqd_t mqClient) всех пользователей:
		if (strcmp("users", substr1) == 0) {
			werase(subwndUsers);
			int cntUsers = 0;
			int lb = bytesForCopy + 1; //левая граница в inBuffer, от которой будет идти поиск следующей ';'
			int rb = -1; //правая граница в inBuffer, которая указывает на новый ';'
			while(1) {
				rb = strcspn(inBuffer + lb, ";"); //узнать номер символа ';'
				//printf("inBuffer + lb = %s\n", inBuffer + lb);
				//printf("rb = %d\n", rb);
				if (!(rb > 0))
					break;
				char username[rb + 1]; 
				strncpy(username, inBuffer + lb, rb); //скопировать имя пользователя
				//printf("username = %s\n", username);
				wmove(subwndUsers, cntUsers++, 0);
				wprintw(subwndUsers, username);
				lb += rb + 1; //левая граница смещается вправо на длину имени и +1 из-за ';'
				rb = -1; //удалить?
			}
			
			wmove(subwndInput, 0, 0);
			wrefresh(subwndUsers); //обновить окно с именами пользователей
			wrefresh(subwndInput); //чтобы показывался курсор
			continue; //начать новую итерацию цикла while(1)
		}       
	   
		/// ........
	   	char* substr2 = malloc(sizeof(inBuffer) - sizeof(substr1));
		strcpy(substr2, inBuffer + bytesForCopy + 1);
		//printf("substr2 = %s\n", substr2);
		//TODO вывод логов в файл
	   	
		wattron(subwndChat, COLOR_PAIR(3));
	   	wmove(subwndChat, rowsInChat++, 0);
	   	//wrefresh(subwndChat);
		wprintw(subwndChat, substr1);
		wattron(subwndChat, COLOR_PAIR(1));
		wmove(subwndChat, rowsInChat++, 0);
		wprintw(subwndChat, substr2);
		wrefresh(subwndChat);
		//TODO проверка на то, выходит ли rowsInChat за границы окна
		//если выходит, то стереть все символы и написать сообщение снова
		
		wmove(subwndInput, 0, 0);
		wrefresh(subwndInput);
		
		//sleep(100);
	}
}

int main (int argc, char **argv)
{
	initscr(); //начало работы с ncurses
	signal(SIGWINCH, Handling_SIGWINCH); 
	//cbreak(); //...
	//curs_set(TRUE); //курсор видимый 
	//noecho(); //отключить вывод вводимых символов
	
	struct winsize size; //структура для размеров окна
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); //получить размеры окна
	int mid_of_terminal = size.ws_col / 2;
	
	//--------- ЦВЕТ ---------//
	start_color(); //начать работу с цветом терминала (ncurses)
	init_pair(1, COLOR_WHITE, COLOR_BLACK); //цвет для
	init_pair(2, COLOR_YELLOW, COLOR_BLACK); //цвет для
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	
	//--------- СОЗДАНИЕ ОКОН  ---------//
	WINDOW *wndChat, *wndUsers, *wndInput;
	WINDOW *subwndChat, *subwndUsers, *subwndInput;
	int currentHeight, currentWidth;
	//первое окно:
	int status = CreateWindow(&wndChat, &subwndChat, 
		size.ws_row - BOTTOM_WND_HEIGHT, size.ws_col - RIGHT_WND_WIDTH, //размер окна
		0, 0, //координаты левого верхнего угла
		" Chat ");
	if (status) {
		printf("error in CreateWindow()\n");
		exit(EXIT_FAILURE);
	}
	//второе окно:
	status = CreateWindow(&wndUsers, &subwndUsers, 
		size.ws_row - BOTTOM_WND_HEIGHT, RIGHT_WND_WIDTH, //размер окна
		0, size.ws_col - RIGHT_WND_WIDTH, //координаты левого верхнего угла
		" Users ");
	if (status) { 
		printf("error in CreateWindow()\n");
		exit(EXIT_FAILURE);
	}
	//3-ье окно:
	status = CreateWindow(&wndInput, &subwndInput, 
		BOTTOM_WND_HEIGHT, size.ws_col, //размер окна
		size.ws_row - BOTTOM_WND_HEIGHT, 0, //координаты левого верхнего угла
		" Your message (press Enter to send) ");
	if (status) {
		printf("error in CreateWindow()\n");
		exit(EXIT_FAILURE);
	}	

	wmove(subwndInput, 0, 0);
	//==========================================================//
	/// mq = message queue (очередь сообщений)
    char clientName[64];
    sprintf (clientName, "/sp-example-client-%d", getpid());
    mqd_t mqServer, mqClient; //дескрипторы очередей сообщений

	/// атрибуты очереди сообщений:
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

	/// открытие очереди сообщений клиента, через нее будут приниматься сообщения от сервера:
    if ((mqClient = mq_open (clientName, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror("error in mq_open() for client");
        exit(EXIT_FAILURE);
    }

	/// открытие очереди сообщений сервера, она должна быть уже создана:
    if ((mqServer = mq_open (SERVER_QUEUE_NAME, O_WRONLY)) == -1) {
        perror("error in mq_open() for server");
        exit(EXIT_FAILURE);
    }

	/// отправка сообщения для присоединения к чату (приоритет 10):
	char joinMessage[6 + strlen(clientName)];
	strcpy(joinMessage, "join;"); //скопировать строку
	strcat(joinMessage, clientName); //добавить строку в конец существующей
    if (mq_send(mqServer, joinMessage, strlen(joinMessage) + 1, 10) == -1) {
        perror("error in mq_send(), joinMessage");
        exit(EXIT_FAILURE);
    }

	//получить дефолтные значения атрибутов:
	pthread_attr_t threadAttr;
	pthread_attr_init(&threadAttr);
	
	/// параметры для потока:
	struct ParamsForThread pft;
	pft.mqClient = mqClient;
	pft.subwndChat = subwndChat;
	pft.subwndInput = subwndInput;
	pft.subwndUsers = subwndUsers;
	void *ptrToStruct = &pft;
	
	/// создать поток, в котором будут обрабатываться получаемые сообщения и обновляться экран:
	pthread_t threadForReceiveMessages;
	if (pthread_create(&threadForReceiveMessages, &threadAttr, FuncForThread, ptrToStruct)) {
		printf("error: can't create pthread\n");
		exit(1);
	}

    while (1) {//(fgets(temp_buf, 2, stdin)) {
		/// считать сообщение, которое для чата:
		char message[MAX_MSG_SIZE - strlen(clientName)];
		wgetnstr(subwndInput, message, MAX_MSG_SIZE - strlen(clientName));//fgets(message, MAX_MSG_SIZE - strlen(clientName), stdin);
		//printf("message = %s\n", message);
		
		
		/// создать сообщение (буфер), которое будет передаваться:
    	char chatMessage[MAX_MSG_SIZE];
    	strcpy(chatMessage, clientName); //скопировать строку
    	strcat(chatMessage, ";"); //добавить строку в конец существующей
    	strcat(chatMessage, message); //добавить строку в конец существующей
    	//printf("chatMessage = %s\n", chatMessage);
    	
    	/// отправить это сообщение (приоритет 5):
        if (mq_send(mqServer, chatMessage, strlen(chatMessage) + 1, 5) == -1) {
            perror ("error in mq_send(), chatMessage");
            continue;
        }
		
		werase(subwndInput);
		wmove(subwndInput, 0, 0);
		wrefresh(subwndInput);
    }


    if (mq_close (mqClient) == -1) {
        perror ("Client: mq_close");
        exit (1);
    }

    if (mq_unlink (clientName) == -1) {
        perror ("Client: mq_unlink");
        exit (1);
    }
    printf ("Client: bye\n");


	//удалить структуры окон
	delwin(subwndChat), delwin(subwndUsers), delwin(subwndUsers);  
	delwin(wndChat), delwin(wndUsers), delwin(wndInput); 
	endwin(); //конец работы с ncurses
    exit(EXIT_SUCCESS);
}
