#include "header.h"
#include <pthread.h> 
#include <mqueue.h> //для очереди сообщений

struct messageInMq {
	struct sockaddr_in client; //end-point клиента
	char frame[FRAME_SIZE]; //кадр данных (т.к. передача на канальном уровне)
}; //сообщение, которое передается созданным потокам

/// функция для потока, в которой ожидается дескриптор сокета,
/// через который надо общаться с клиентом:
void *FuncForThread(void *param) 
{	
	/// получить дескриптор сокета из параметра функции:
	int *fdSocket = (int *)param; 

	/// подключиться к очереди сообщений:
	mqd_t mqDescr;
	if ((mqDescr = mq_open(MQ_NAME, O_RDONLY)) == 1) {
	    perror("error in mq_open() for created thread");
	    exit(EXIT_FAILURE);
	}

	while (1) {
		/// получить сообщение через очередь (блокирующий вызов):
	    char inBuffer[256] = {0};
	    if (mq_receive(mqDescr, inBuffer, 256, NULL) == -1) {
	        perror("error in mq_receive() for created thread");
	   		exit(EXIT_FAILURE);
	    }

	    /// читаем сообщение не как char*, а как структуру:
	    struct messageInMq *message = (struct messageInMq *)&inBuffer;
	    printf("received packet from %s:%d\n", 
	    	   inet_ntoa(message->client.sin_addr), 
	    	   ntohs(message->client.sin_port));
	    printf("frame data: %s\n\n", message->frame);
	    
		/// обработка кадра от клиента (сделать эхо-ответ):	
		strcat(message->frame, " echo-reply"); //добавить строку в конец существующей
		int sizeClient = sizeof(message->client);
		int ret = sendto(*fdSocket, message->frame, sizeof(message->frame), 
	                     0, (struct sockaddr*)&message->client, 
	                     sizeClient);
		if (ret == -1) {
			perror("error in sendto()");
			exit(EXIT_FAILURE);
		}
	} //конец while(1)
}

int main(void) 
{	
	/// заполнить структуру с адресом для сокета:
	struct sockaddr_in server;
	server.sin_family = AF_INET; //sun = socket UNIX, sin = socket inet
	server.sin_port = htons(PORT); //перевести в big endian
	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	
	/// Шаг 1. Создать файловый дескриптор сокета:
	int fdSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (fdSocket == -1) {
		perror("error in socket()");
		exit(EXIT_FAILURE);
	}
	
	/// Шаг 2. Связывание сокета и адреса:
	int ret = bind(fdSocket, (const struct sockaddr *)&server, 
	               sizeof(struct sockaddr_in));
	if (ret == -1) {
		perror("error in bind()");
		exit(EXIT_FAILURE);
	}

	/// атрибуты очереди сообщений:
    struct mq_attr mqAttr;
    mqAttr.mq_flags = 0;
    mqAttr.mq_maxmsg = MAX_MESSAGES;
    mqAttr.mq_msgsize = sizeof(struct messageInMq);
    mqAttr.mq_curmsgs = 0;
	
    /// создание очерди сообщений:
	mqd_t mqDescr = mq_open(MQ_NAME, O_WRONLY | O_CREAT, 
		                    QUEUE_PERMISSIONS, &mqAttr);
	if (mqDescr == -1) {
        perror("error in mq_open() for main thread");
        exit(EXIT_FAILURE);
    }

	/// переменные для потоков:
	pthread_t thrId[CNT_THREADS]; //дескрипторы потоков 

	/// получить стандартные значения атрибутов для потоков:
	pthread_attr_t thrAttr;
	pthread_attr_init(&thrAttr);

	/// создать очереди сообщений и потоки:
	for (int i = 0; i < CNT_THREADS; i++) {
	    /// создание потока:
	    void *param = (void *)&fdSocket;
		if (pthread_create(&thrId[i], &thrAttr, FuncForThread, param)) {
			fprintf(stderr, "error: can't create pthread\n");
			exit(EXIT_FAILURE);
		}
	}
	
	/// цикл обработки подключений:
	for (int i = 0; ; i++) {
		/// end-point клиента:
		struct sockaddr_in client;
		int sizeClient = sizeof(client);

		/// Шаг 3. Ожидание входящих сообщений:
		char inBuffer[FRAME_SIZE] = {0};
		ret = recvfrom(fdSocket, inBuffer, FRAME_SIZE, 
		               0, (struct sockaddr *)&client, 
		               (socklen_t *)&sizeClient);
		if (ret == -1) {
			perror("error in recvfrom()");
			exit(EXIT_FAILURE);
		}

		/// отправить end-poing клиента и его пакет в очередь сообщений
		/// с которым будет разбираться один из потоков (под-сервер): 
		struct messageInMq message;
		message.client = client;
		strcpy(message.frame, inBuffer);
		ret = mq_send(mqDescr, (char *)&message, 
			          sizeof(message), 5);
		if (ret == -1) {
		    perror("error in mq_send() for main thread");
		    exit(EXIT_FAILURE);
		}
	}	
	
	/// удалить сокет:
	close(fdSocket);
	
	exit(EXIT_SUCCESS);
}
