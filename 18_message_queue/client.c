#include "header.h"

#include <termios.h>
#include <sys/ioctl.h> //для обработки сигналов
#include <signal.h> //для соединения сигнала и функции-обработчика
#include <curses.h> //ncurses
#include <pthread.h>

//TODO посмотреть какие библиотеки для чего нужны

#define RIGHT_WND_WIDTH 20
#define BOTTOM_WND_HEIGHT 5

void Handling_SIGWINCH(int signo) 
{ //обработчик сигнала SIGWINCH
	struct winsize size; //структура для размеров окна
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); //получить размеры окна
	resizeterm(size.ws_row, size.ws_col); //изменить размеры окна
}

int CreateWindow(WINDOW **wnd, WINDOW **subwnd, int height, int width, int x, int y, char* text) 
{
	/// создание окна:
	*wnd = newwin(height, width, x, y); //создать окно(высота, ширина, координаты левого верхнего угла)
	if (*wnd == NULL)
		return 1;
	wborder(*wnd, '|', '|', '=', '=', '|', '|', '|', '|'); //границы окна
	
	/// добавление надписи:
	wmove(*wnd, 0, 4);
	wattron(*wnd, COLOR_PAIR(2));
	wprintw(*wnd, text);
	wattron(*wnd, COLOR_PAIR(1));
	wmove(*wnd, 1, 1);
	
	/// создание под-окна (по сути это окно без рамок):
	*subwnd = derwin(*wnd, height - 2, width - 2, 1, 1); 
	if (*subwnd == NULL)
		return 1;
	wrefresh(*wnd);
	return 0;
}

struct ParamsForThread
{ //структура, для передачи параметоров в функцию ниже
	mqd_t mqClient;
	char *clientName;
	WINDOW *subwndChat;
	WINDOW *subwndInput;
	WINDOW *subwndUsers;
};

void *FuncForThread(void *param) 
{ //функция для потока, в котором будут приниматься и обрабатываться сообщения из очереди сообщений
	/// "извлечение" переменных:
	struct ParamsForThread *pft = (struct ParamsForThread*) param; 
	mqd_t mqClient = pft->mqClient; //дескриптор очереди сообщений
	char *clientName = pft->clientName;
	WINDOW *subwndChat = pft->subwndChat;
	WINDOW *subwndInput = pft->subwndInput;
	WINDOW *subwndUsers = pft->subwndUsers;

	/// цикл для приема и обработки сообщений:
	int rowsInChat = 0; //сколько строк использовано в окне с чатом
	while (1) {
		/// получить сообщение от сервера (блокирующий вызов): 
		char inBuffer[MAX_MSG_SIZE]; //буфер для сообщения
		if (mq_receive(mqClient, inBuffer, MSG_BUFFER_SIZE, NULL) == -1) {
		    perror("error in mq_receive()");
		    exit(EXIT_FAILURE);
		}
		//printf("received message = %s\n", inBuffer);
		
		/// разбить полученное сообщение на 2 подстроки, которые всегда разделяются ';'
		long bytesForCopy = strcspn(inBuffer, ";"); //узнать номер символа ';'
		char *substr1 = malloc(sizeof(char) * (bytesForCopy + 1)); //создать переменную для подстроки 1
		strncpy(substr1, inBuffer, bytesForCopy); //скопировать содержимое подстроки 1
		//printf("substr1 = %s\n", substr1);
	   
		/// обработка сообщения "users" - нужно вывести имена (mqd_t mqClient) всех пользователей:
		if (strcmp("users", substr1) == 0) {
			//printf("substr2 = %s\n", substr2);
			werase(subwndUsers);
			int cntUsers = 0; //количество пользователей
			int lb = bytesForCopy + 1; //левая граница в inBuffer, от которой будет идти поиск следующей ';'
			int rb = -1; //правая граница в inBuffer, которая указывает на новый ';'
			while(1) { //цикл, пока есть несчитанные имена пользователей
				rb = strcspn(inBuffer + lb, ";"); //узнать номер символа ';'
				if (rb <= 0) //если ';' не найдено, то все имена записаны
					break;
				char username[rb + 1]; 
				strncpy(username, inBuffer + lb, rb); //скопировать имя пользователя
				//printf("username = %s\n", username);
				wmove(subwndUsers, cntUsers++, 0);
				if (strncmp(username, clientName, rb) == 0) { //если пишется имя текущего пользователя  	
					wattron(subwndUsers, COLOR_PAIR(3)); //тогда выделить это имя зеленым цветом
					wprintw(subwndUsers, username);
					wattron(subwndUsers, COLOR_PAIR(1));
				}
				else
					wprintw(subwndUsers, username);
				lb += rb + 1; //левая граница смещается вправо на длину имени и +1 из-за ';'
			}
			
			wmove(subwndInput, 0, 0); //убрать курсор из окна с пользователями
			wrefresh(subwndUsers);
			wrefresh(subwndInput); 
			continue; //вернуться на начало цикла (ожидать новое сообщение)
		}       
	   
		/// если выполнение кода дошло до сюда, то это значит, что получено сообщение для чата
		/// у которого 1-ая подстрока = имени отправителя сообщения
	   	char* substr2 = malloc(sizeof(inBuffer) - sizeof(substr1));
		strcpy(substr2, inBuffer + bytesForCopy + 1);
		//printf("substr2 = %s\n", substr2);
		//TODO вывод логов в файл
	   	
	   	/// написать имя пользователя и его сообщение:
		wattron(subwndChat, COLOR_PAIR(3)); //зеленый цвет текста
	   	wmove(subwndChat, rowsInChat++, 0);
		wprintw(subwndChat, substr1);
		wattron(subwndChat, COLOR_PAIR(1)); //белый цвет текста
		wmove(subwndChat, rowsInChat++, 0);
		wprintw(subwndChat, substr2);
		wrefresh(subwndChat);
		//TODO проверка на то, выходит ли rowsInChat за границы окна
		//если выходит, то стереть все символы и написать сообщение снова
		
		wmove(subwndInput, 0, 0);
		wrefresh(subwndInput);
	}
}

int main (int argc, char **argv)
{
	//=========================================================================//
	// создание псевдографики на ncurses для чата, имен пользователей и пр.    //
	//=========================================================================//

	initscr(); //начало работы с ncurses
	signal(SIGWINCH, Handling_SIGWINCH); 
	//cbreak(); //...
	//curs_set(TRUE); //курсор видимый 
	//noecho(); //отключить вывод вводимых символов
	
	struct winsize size; //структура для размеров окна
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); //получить размеры окна
	int mid_of_terminal = size.ws_col / 2;
	
	/// цвета ncurses:
	start_color(); //начать работу с цветом терминала (ncurses)
	init_pair(1, COLOR_WHITE, COLOR_BLACK); //цвет для обычного текста
	init_pair(2, COLOR_YELLOW, COLOR_BLACK); //цвет для обозначения окон
	init_pair(3, COLOR_GREEN, COLOR_BLACK); //для имен пользователей
	
	/// создание окон:
	WINDOW *wndChat, *wndUsers, *wndInput;
	WINDOW *subwndChat, *subwndUsers, *subwndInput;
	int currentHeight, currentWidth;
	//1-ое окно:
	int status = CreateWindow(&wndChat, &subwndChat, 
		size.ws_row - BOTTOM_WND_HEIGHT, size.ws_col - RIGHT_WND_WIDTH, //размер окна
		0, 0, //координаты левого верхнего угла
		" Chat ");
	if (status) {
		fprintf(stderr, "error in CreateWindow() for chat\n");
		exit(EXIT_FAILURE);
	}
	//2-ое окно:
	status = CreateWindow(&wndUsers, &subwndUsers, 
		size.ws_row - BOTTOM_WND_HEIGHT, RIGHT_WND_WIDTH, //размер окна
		0, size.ws_col - RIGHT_WND_WIDTH, //координаты левого верхнего угла
		" Users ");
	if (status) { 
		fprintf(stderr, "error in CreateWindow() for users\n");
		exit(EXIT_FAILURE);
	}
	//3-ье окно:
	status = CreateWindow(&wndInput, &subwndInput, 
		BOTTOM_WND_HEIGHT, size.ws_col, //размер окна
		size.ws_row - BOTTOM_WND_HEIGHT, 0, //координаты левого верхнего угла
		" Your message (press Enter to send, Ctrl-C to exit) ");
	if (status) {
		fprintf(stderr, "error in CreateWindow() for input\n");
		exit(EXIT_FAILURE);
	}	

	wmove(subwndInput, 0, 0); //передвинуть курсор
	
	//=========================================================================//
	// начало работы с очередями сообщений                                     //
	//=========================================================================//
	
	/// создание очереди сообщений (mq = message queue):
    char* clientName = malloc(sizeof(char) * 64);
    sprintf(clientName, "/sp-client-%d", getpid()); //если не написать "sp" в начале, 
	                                                //то происходят баги 
    mqd_t mqServer, mqClient; //дескрипторы очередей сообщений

	/// атрибуты очереди сообщений:
    struct mq_attr mqAttr;
    mqAttr.mq_flags = 0;
    mqAttr.mq_maxmsg = MAX_MESSAGES;
    mqAttr.mq_msgsize = MAX_MSG_SIZE;
    mqAttr.mq_curmsgs = 0;

	/// открытие очереди сообщений клиента, через нее будут приниматься сообщения от сервера:
    if ((mqClient = mq_open (clientName, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &mqAttr)) == -1) {
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

	//=========================================================================//
	// создание потока, в котором будут приниматься и обрабатываться сообщения //
	//=========================================================================//

	/// получить стандартные значения атрибутов для потоков:
	pthread_attr_t threadAttr;
	pthread_attr_init(&threadAttr);
	
	/// переменные для потока:
	struct ParamsForThread pft;
	pft.mqClient = mqClient;
	pft.clientName = clientName;
	pft.subwndChat = subwndChat;
	pft.subwndInput = subwndInput;
	pft.subwndUsers = subwndUsers;
	void *ptrToStruct = &pft;
	
	/// создать поток:
	pthread_t threadForReceiveMessages;
	if (pthread_create(&threadForReceiveMessages, &threadAttr, FuncForThread, ptrToStruct)) {
		fprintf(stderr, "error: can't create pthread\n");
		exit(EXIT_FAILURE);
	}
	
	char ch = 0; //переменная для wgetch() 
	while ((ch = wgetch(subwndInput)) != 27) { //27 - код клавиши ESC
		/// считать сообщение, которое вводит пользователь для чата:
		char *message = malloc(sizeof(char) * (MAX_MSG_SIZE - strlen(clientName)));
		message[0] = ch;
		wgetnstr(subwndInput, message + 1, MAX_MSG_SIZE - strlen(clientName) - 1); //-1 из-за ch
		
		//printf("message = %s\n", message);
		
		/// создать сообщение (буфер), которое будет передаваться:
    	char chatMessage[MAX_MSG_SIZE];
    	strcpy(chatMessage, clientName); //скопировать строку
    	strcat(chatMessage, ";"); //добавить строку в конец существующей
    	strcat(chatMessage, message); //добавить строку в конец существующей
    	//printf("chatMessage = %s\n", chatMessage);
    	
    	/// отправить это сообщение в очередь сообщений (приоритет 5):
        if (mq_send(mqServer, chatMessage, strlen(chatMessage) + 1, 5) == -1) {
            perror("error in mq_send(), chatMessage");
            continue;
        }
		
		/// полностью стереть сообщение, т.к. оно попадает в окно чат
		werase(subwndInput);
		wmove(subwndInput, 0, 0);
		wrefresh(subwndInput);
    }
    
    /// отправка сообщения о выходе из чата (приоритет 10):
	char exitMessage[6 + strlen(clientName)];
	strcpy(exitMessage, "exit;"); //скопировать строку
	strcat(exitMessage, clientName); //добавить строку в конец существующей
    if (mq_send(mqServer, exitMessage, strlen(exitMessage) + 1, 10) == -1) {
        perror("error in mq_send(), exitMessage");
        exit(EXIT_FAILURE);
    }

	/*
    if (mq_close (mqClient) == -1) {
        perror ("Client: mq_close");
        exit (1);
    }

    if (mq_unlink (clientName) == -1) {
        perror ("Client: mq_unlink");
        exit (1);
    }
	*/
	delwin(subwndChat), delwin(subwndUsers), delwin(subwndUsers);  
	delwin(wndChat), delwin(wndUsers), delwin(wndInput); 
	endwin(); //конец работы с ncurses
	
    exit(EXIT_SUCCESS);
}
