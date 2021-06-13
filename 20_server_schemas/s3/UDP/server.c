#include "header.h"
#include <pthread.h> 
//#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h> //для очереди сообщений

struct messageInMq {
	struct sockaddr_in client; //end-point клиента
	char frame[FRAME_SIZE]; //кадр данных (т.к. передача на канальном уровне)
}; //сообщение, которое передается созданным потокам

/// функция для потока, в которой ожидается дескриптор сокета,
/// через который надо общаться с клиентом:
void *FuncForThread(void *param) 
{	
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

	    //aaaaaaaaaaaaa
	    struct messageInMq *message = (struct messageInMq *)&inBuffer;
	    printf("aaaa %s:%d\n", 
	    	   inet_ntoa(message->client.sin_addr), 
	    	   ntohs(message->client.sin_port));
	    printf("frame: %s\n", message->frame);
	    

	    /*
	    /// преобразовать сообщение в дескриптор сокета:
	    int fdDataSocket = atoi(inBuffer);
	    printf("received fdDataSocket == %d\n", fdDataSocket);
	    
		/// обработка пакетов (кадров?) клиента:	
		while(1) {
	    	char buffer[BUFFER_SIZE] = {0};
			/// ожидание пакета с данными:
			int ret = recv(fdDataSocket, buffer, BUFFER_SIZE, 0);
			if (ret == -1) {
				perror("error in recv()");
				exit(EXIT_FAILURE);
			} 
			printf("\nreceived: %s\n", buffer);
			
			/// если получена команда для завершения общения: 
			if (strncmp(buffer, "END", BUFFER_SIZE) == 0) {
				close(fdDataSocket);
				break; //выход из цикла
			}
			
			/// иначе сделать эхо-ответ:
			strcat(buffer, " echo-reply"); //добавить строку в конец существующей
			ret = send(fdDataSocket, buffer, BUFFER_SIZE, 0);
			if (ret == -1) {
				perror("error in send()");
				exit(EXIT_FAILURE);
			} 
			printf("    sent: %s\n", buffer);
		}
		*/
	}
}

int main(void) 
{	
	/// заполнить структуру с адресом для сокета:
	struct sockaddr_in server;
	server.sin_family = AF_INET; //sun = socket UNIX, sin = socket inet
	server.sin_port = htons(7777); //перевести в big endian
	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	
	/// Шаг 1. Создать файловый дескриптор сокета:
	int fdConnectSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (fdConnectSocket == -1) {
		perror("error in socket()");
		exit(EXIT_FAILURE);
	}
	
	/// Шаг 2. Связывание сокета и адреса:
	int ret = bind(fdConnectSocket, (const struct sockaddr *)&server, 
	               sizeof(struct sockaddr_in));
	if (ret == -1) {
		perror("error in bind()");
		exit(EXIT_FAILURE);
	}
	
	/// получить стандартные значения атрибутов для потоков:
	pthread_attr_t thrAttr;
	pthread_attr_init(&thrAttr);

	/// атрибуты очереди сообщений:
    struct mq_attr mqAttr;
    mqAttr.mq_flags = 0;
    mqAttr.mq_maxmsg = MAX_MESSAGES;
    mqAttr.mq_msgsize = MAX_MSG_SIZE;
    mqAttr.mq_curmsgs = 0;
	
    /// создание очерди сообщений:
	mqd_t mqDescr = mq_open(MQ_NAME, O_WRONLY | O_CREAT, QUEUE_PERMISSIONS, &mqAttr);
	if (mqDescr == -1) {
        perror("error in mq_open() for main thread");
        exit(EXIT_FAILURE);
    }

	/// переменные для потоков:
	pthread_t thrId[CNT_THREADS]; //дескрипторы потоков 
	int fdDataSocket[CNT_THREADS] = {0}; //файловые дескрипторы сокетов

	/// создать очереди сообщений и потоки:
	for (int i = 0; i < CNT_THREADS; i++) {
	    /// создание потока:
		if (pthread_create(&thrId[i], &thrAttr, FuncForThread, NULL)) {
			fprintf(stderr, "error: can't create pthread\n");
			exit(EXIT_FAILURE);
		}
	}
	
	/// цикл обработки подключений:
	for (int i = 0; ; i++) {
		struct sockaddr_in client;
		// int slen = sizeof(si_other);
		// //try to receive some data, this is a blocking call
		// if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
		// {
		// 	die("recvfrom()");
		// }
		
		//print details of the client/peer and the data received
		// printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		// printf("Data: %s\n" , buf);
		
		//now reply the client with the same data
		// if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
		// {
		// 	die("sendto()");
		// }

		/// Шаг 3. Ожидание входящих сообщений:
		char buffer[BUFFER_SIZE] = {0};
		int sizeClient = sizeof(client);
		ret = recvfrom(fdConnectSocket, buffer, BUFFER_SIZE, 
		               0, (struct sockaddr *)&client, 
		               (socklen_t *)&sizeClient);
		if (ret == -1) {
			perror("error in recvfrom()");
			exit(EXIT_FAILURE);
		}
		printf("Received packet from %s:%d\n", 
			   inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		printf("Data: %s\n", buffer);


		///aaaaaaaaaa
		ret = sendto(fdConnectSocket, buffer, ret, 
		             0, (struct sockaddr*)&client, 
		             sizeClient);
		if (ret == -1) {
			perror("error in sendto()");
			exit(EXIT_FAILURE);
		}
		
		/// отослать сообщение через mq:
		//char outBuffer[8] = {0};
		//sprintf(outBuffer, "%d", fdDataSocket[i]);

		//ааааааааааа
		struct messageInMq message;
		message.client = client;
		strcpy(message.frame, buffer);
		if (mq_send(mqDescr, (char *)&message, 
			sizeof(message), 5) == -1) {
		    perror("error in mq_send() for main thread");
		    exit(EXIT_FAILURE);
		}

		/// вывод лога:
		//printf("send message \"%s\" in message queue\n", 
		//       fdDataSocket[i]);
	}	
	
	/// удалить сокет:
	close(fdConnectSocket);
	
	exit(EXIT_SUCCESS);
}
