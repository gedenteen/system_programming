#include "header.h"

int main (int argc, char **argv)
{
	/// создание очереди сообщений (mq = message queue):
    mqd_t mqServer, mqClient; //дескрипторы очередей сообщений
    printf("server init\n");

	/// атрибуты очереди сообщений:
    struct mq_attr mqAttr;
    mqAttr.mq_flags = 0;
    mqAttr.mq_maxmsg = MAX_MESSAGES;
    mqAttr.mq_msgsize = MAX_MSG_SIZE;
    mqAttr.mq_curmsgs = 0;

	/// создание сервера для очереди сообщений, который будет принимать сообщения от клиентов:
    if ((mqServer = mq_open(SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &mqAttr)) == -1) {
        perror ("error in mq_open() for server");
        exit(EXIT_FAILURE);
    }
    
    /// буферы для приема и отправки сообщений:
    char inBuffer[MSG_BUFFER_SIZE];
    char outBuffer[MSG_BUFFER_SIZE];

	/// переменные для хранения имен клиентов:
	char** mqClientsTable = malloc(sizeof(char*));
	int countClients = 0;
	
	/// обработка сообщений:
    while (1) {
        /// получить сообщение (с самым высоким приоритетом, самое старое):
        if (mq_receive(mqServer, inBuffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("error in mq_receive()");
       		exit(EXIT_FAILURE);
        }
		printf("message received = %s\n", inBuffer);

		/// разбить полученное сообщение на 2 подстроки, которые всегда должны разделяться ';'
		long bytesForCopy = strcspn(inBuffer, ";"); //узнать номер символа ';'
		char* substr1 = malloc(sizeof(char) * bytesForCopy); //создать переменную для подстроки 1
		strncpy(substr1, inBuffer, bytesForCopy); //скопировать содержимое подстроки 1
		printf("substr1 = %s\n", substr1);
		
		/// если подстрока 1 == "join", то клиент хочет подключиться к серверу
		if (strcmp("join", substr1) == 0) {
			countClients++; //увеличить кол-во клиентов, о которых знает сервер
			mqClientsTable = realloc(mqClientsTable, sizeof(char*) * countClients); //добавить строку в таблицу
			mqClientsTable[countClients - 1] = malloc( //размер новой строки равен размеру имени клиента
				strlen(inBuffer - bytesForCopy) * sizeof(char));
			strcpy(mqClientsTable[countClients - 1], inBuffer + bytesForCopy + 1); //скопировать имя из подстроки 2
			printf("new client %s joined\n", mqClientsTable[countClients - 1]); 
			
			/// отправка сообщения "users" всем клиентам:
			strcpy(outBuffer, "users;");
			for (int i = 0; i < countClients; i++) {
				strcat(outBuffer, mqClientsTable[i]);
				strcat(outBuffer, ";");
			}
			printf("\"users\" message = %s\n", outBuffer);
			for (int i = 0; i < countClients; i++) {
		    	printf("prepare to send message \"users\" for client %s\n", mqClientsTable[i]);
				if ((mqClient = mq_open(mqClientsTable[i], O_WRONLY)) == 1) {
				    perror("error in mq_open(), \"users\" message");
				    exit(EXIT_FAILURE);
				}

				if (mq_send (mqClient, outBuffer, strlen(outBuffer) + 1, 5) == -1) {
				    perror("error in mq_send(), \"users\" message");
				    exit(EXIT_FAILURE);
				}
		    }
			
			continue; //начать новую итерацию цикла
		}
		
		/// если подстрока 1 == "exit", то клиент закрывает свою программу, надо удалить запись о нем:
		if (strcmp("exit", substr1) == 0) {
			// TODO
			continue;
		}
		
		/// если программа дошла до сюда, значит, пользователь отправил сообщение в чат
		/// подстрока 1 - это имя клиента, подстрока 2 - его сообщение:
		char substr2[MAX_MSG_SIZE];
		strcpy(substr2, inBuffer + bytesForCopy + 1);
		printf("substr2 = %s\n", substr2);

        /// отправить это сообщение всем клиентам, чтобы они обновили окно с чатом:
        for (int i = 0; i < countClients; i++) {
        	printf("prepare to send message for client %s\n", mqClientsTable[i]);
		    if ((mqClient = mq_open(mqClientsTable[i], O_WRONLY)) == 1) {
		        perror("error in mq_send(), chat message");
				exit(EXIT_FAILURE);
		    }

		    if (mq_send (mqClient, inBuffer, strlen(inBuffer) + 1, 5) == -1) {
		        perror("error in mq_send(), chat message");
				exit(EXIT_FAILURE);
		    }
        }
    }
}
