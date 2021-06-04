#include "header.h"

int main (int argc, char **argv)
{
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

        // receive response from server
		char inBuffer[MAX_MSG_SIZE];
        if (mq_receive (mqClient, inBuffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Client: mq_receive");
            exit (1);
        }
        // display token received from server
        printf ("Client: Token received from server: %s\n\n", inBuffer);

        printf ("Ask for a token (Press ): ");
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

    exit (0);
}
