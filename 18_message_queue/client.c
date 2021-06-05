#include "header.h"

#include <termios.h>
#include <sys/ioctl.h> //для обработки сигналов
#include <signal.h> //для соединения сигнала и функции-обработчика
#include <curses.h> //ncurses

//TODO посмотреть какие библиотеки для чего нужны

#define RIGHT_WND_WIDTH 23
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
	
	*subwnd = derwin(*wnd, height - 2, width - 2, 1, 1); //под-окно, чтобы не стирать границы окна
	if (*subwnd == NULL)
		return 1;
	wrefresh(*wnd);
	return 0;
}

int main (int argc, char **argv)
{
	initscr();
	signal(SIGWINCH, Handling_SIGWINCH);
	cbreak(); //...
	curs_set(TRUE); //курсор видимый 
	noecho(); //отключить вывод вводимых символов
	
	struct winsize size; //структура для размеров окна
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); //получить размеры окна
	int mid_of_terminal = size.ws_col / 2;
	
	//--------- ЦВЕТ ---------//
	start_color(); //начать работу с цветом терминала (ncurses)
	init_pair(1, COLOR_WHITE, COLOR_BLACK); //цвет для остальных файлов
	init_pair(2, COLOR_YELLOW, COLOR_BLACK); //цвет для выделения выбранного файла
	
	//--------- СОЗДАНИЕ ОКОН  ---------//
	WINDOW *wndChat, *wndUsers, *wndInput;
	WINDOW *subwndChat, *subwndUsers, *subwndInput;
	int currentHeight, currentWidth;
	//первое окно:
	int status = CreateWindow(&wndChat, &subwndChat, 
		size.ws_row - BOTTOM_WND_HEIGHT, size.ws_col - RIGHT_WND_WIDTH, //размер окна
		0, 0,
		" Chat "); //координаты левого верхнего угла
	if (status) {
		printf("error\n");
		//exit(EXIT_FAILURE);
	}
	//второе окно:
	status = CreateWindow(&wndUsers, &subwndUsers, 
		size.ws_row - BOTTOM_WND_HEIGHT, RIGHT_WND_WIDTH, //размер окна
		0, size.ws_col - RIGHT_WND_WIDTH,
		" Users "); //координаты левого верхнего угла
	if (status) { 
		printf("error\n");
		//exit(EXIT_FAILURE);
	}
	//3-ье окно:
	status = CreateWindow(&wndInput, &subwndInput, 
		BOTTOM_WND_HEIGHT, size.ws_col, //размер окна
		size.ws_row - BOTTOM_WND_HEIGHT, 0,
		" Your message (press Enter to send) "); //координаты левого верхнего угла
	if (status) {
		printf("error\n");
		//exit(EXIT_FAILURE);
	}	

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

    printf ("Ask for a token (Press <ENTER>): ");
    char temp_buf[10];

    while (fgets(temp_buf, 2, stdin)) {
		/// считать сообщение, которое для чата:
		char message[MAX_MSG_SIZE - strlen(clientName)];
		fgets(message, MAX_MSG_SIZE - strlen(clientName), stdin);
		printf("message = %s\n", message);
		
		/// создать сообщение (буфер), которое будет передаваться:
    	char chatMessage[MAX_MSG_SIZE];
    	strcpy(chatMessage, clientName); //скопировать строку
    	strcat(chatMessage, ";"); //добавить строку в конец существующей
    	strcat(chatMessage, message); //добавить строку в конец существующей
    	printf("chatMessage = %s\n", chatMessage);
    	
    	/// отправить это сообщение (приоритет 5):
        if (mq_send(mqServer, chatMessage, strlen(chatMessage) + 1, 5) == -1) {
            perror ("error in mq_send(), chatMessage");
            continue;
        }

        /// получить сообщение от сервера: 
		char inBuffer[MAX_MSG_SIZE];
        if (mq_receive (mqClient, inBuffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Client: mq_receive");
            exit (1);
        }
       
        ///
       
        // display token received from server
        //printf ("Client: Token received from server: %s\n\n", inBuffer);
        //printf ("Ask for a token (Press ): ");
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
