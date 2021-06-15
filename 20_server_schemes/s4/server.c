#include "header.h"
#include <pthread.h> 
#include <mqueue.h> //для очереди сообщений
#include <sys/poll.h> //для мультиплексированного ввода-вывода

/// Структуры для передачи сообщений по message queue:
struct messageInMqUdp {
	char protocol[4]; //"UDP\0"
	char frame[FRAME_SIZE]; //кадр данных (т.к. передача на канальном уровне)
	struct sockaddr_in client; //end-point клиента
	int fdSockUDP; //сокет, по которому передавать данные
};

struct messageInMqTcp {
	char protocol[4]; //"TCP\0"
	int fdDataSockTCP; //сокет, по которому передавать данные
};

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
	    char inBuffer[512] = {0};
	    if (mq_receive(mqDescr, inBuffer, 512, NULL) == -1) {
	        perror("error in mq_receive() for created thread");
	   		exit(EXIT_FAILURE);
	    }

	    if (strncmp(inBuffer, "UDP", 3) == 0) {
	    	printf("sub-server: this is UDP!\n");

	    	/// читаем сообщение не как char*, а как структуру:
		    struct messageInMqUdp *message = (struct messageInMqUdp *)&inBuffer;
		    printf("received packet from %s:%d\n", 
		    	   inet_ntoa(message->client.sin_addr), 
		    	   ntohs(message->client.sin_port));
		    printf("frame data: %s\n\n", message->frame);
		    
			/// обработка кадра от клиента (сделать эхо-ответ):	
			strcat(message->frame, " echo-reply"); //добавить строку в конец существующей
			int sizeClient = sizeof(message->client);
			int ret = sendto(message->fdSockUDP, message->frame, sizeof(message->frame), 
		                     0, (struct sockaddr*)&message->client, sizeClient);
			if (ret == -1) {
				perror("error in sendto()");
				exit(EXIT_FAILURE);
			}
	    }
	    else if (strncmp(inBuffer, "TCP", 3) == 0) {
	    	printf("sub-server: this is TCP!\n");

	    	/// читаем сообщение не как char*, а как структуру:
			struct messageInMqTcp *message = (struct messageInMqTcp *)&inBuffer;
	    	
	    	while (1) { //цикл бесконечный, пока клиент посылает кадры
		    	char buffer[FRAME_SIZE] = {0};
				/// ожидание кадра с данными:
				int ret = recv(message->fdDataSockTCP, buffer, FRAME_SIZE, 0);
				if (ret == -1) {
					perror("error in recv()");
					exit(EXIT_FAILURE);
				} 
				printf("TCP received: %s\n", buffer);
				
				/// если получена команда для завершения общения: 
				if (strncmp(buffer, "END", FRAME_SIZE) == 0) {
					close(message->fdDataSockTCP);
					break; //выход из цикла
				}
				
				/// иначе сделать эхо-ответ:
				strcat(buffer, " echo-reply"); //добавить строку в конец существующей
				ret = send(message->fdDataSockTCP, buffer, FRAME_SIZE, 0);
				if (ret == -1) {
					perror("error in send()");
					exit(EXIT_FAILURE);
				} 
				printf("    TCP sent: %s\n", buffer);
		    }
		}
	    else {
	    	printf("sub-server: reseived wrong message from MQ\n");
	    	exit(EXIT_FAILURE);
	    }
	} //конец while(1)
}

int main(void) 
{	
	//======================================================//
	// UDP, очередь сообщений, потоки (под-серверы)         //
	//======================================================//

	/// заполнить структуру с адресом для сокета:
	struct sockaddr_in server;
	server.sin_family = AF_INET; //sun = socket UNIX, sin = socket inet
	server.sin_port = htons(PORT); //перевести в big endian
	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	
	/// Шаг 1. Создать файловый дескриптор сокета:
	int fdSockUDP = socket(AF_INET, SOCK_DGRAM, 0);
	if (fdSockUDP == -1) {
		perror("error in socket()");
		exit(EXIT_FAILURE);
	}
	
	/// Шаг 2. Связывание сокета и адреса:
	int ret = bind(fdSockUDP, (const struct sockaddr *)&server, 
	               sizeof(struct sockaddr_in));
	if (ret == -1) {
		perror("error in bind()");
		exit(EXIT_FAILURE);
	}

	/// атрибуты очереди сообщений:
    struct mq_attr mqAttr;
    mqAttr.mq_flags = 0;
    mqAttr.mq_maxmsg = MAX_MESSAGES;
    mqAttr.mq_msgsize = sizeof(struct messageInMqUdp) + 1;
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

	/// создать потоки:
	for (int i = 0; i < CNT_THREADS; i++) {
		if (pthread_create(&thrId[i], &thrAttr, FuncForThread, NULL)) {
			fprintf(stderr, "error: can't create pthread\n");
			exit(EXIT_FAILURE);
		}
	}

	//======================================================//
	// TCP, мультиплексированный ввод-вывод                 //
	//======================================================//

	/// Шаг 1. Создать файловый дескриптор сокета:
	int fdConnectSockTCP = socket(AF_INET, SOCK_STREAM, 0);
	if (fdConnectSockTCP == -1) {
		perror("error in socket()");
		exit(EXIT_FAILURE);
	}
	
	/// Шаг 2. Связывание сокета и адреса:
	ret = bind(fdConnectSockTCP, (const struct sockaddr *) &server, 
	               sizeof(struct sockaddr_in));
	if (ret == -1) {
		perror("error in bind()");
		exit(EXIT_FAILURE);
	}
	
	/// Шаг 3. Ожидание запросов от клиентов:
	if (listen(fdConnectSockTCP, 5) == -1) {
		perror("error in listen()");
		exit(EXIT_FAILURE);
	}

	/// сокет для подключения к клиенту:
	int fdDataSockTCP;

	/// Мультиплексированный ввод-вывод для обоих сокетов:
	struct pollfd fds[2];
	fds[0].fd = fdSockUDP; 
	fds[0].events = POLLIN;
	fds[1].fd = fdConnectSockTCP; 
	fds[1].events = POLLIN;

	//======================================================//
	// цикл обработки подключений                           //
	//======================================================//

	while(1) {
		/// блокирующий вызов, пока один клиент не пришлет кадр (UDP) 
		/// или не подключится (TCP):
		ret = poll(fds, 2, -1);
		if (ret == -1) {
			perror("error in poll()");
			exit(EXIT_FAILURE);
		}

		/// если получен кадр по протоколу UDP:
		if (fds[0].revents & POLLIN) {
			printf("\nreceived a frame by UDP protocol\n");

			/// End-point клиента:
			struct sockaddr_in client;
			int sizeClient = sizeof(client);

			/// Ожидание входящих сообщений:
			char inBuffer[FRAME_SIZE] = {0};
			ret = recvfrom(fdSockUDP, inBuffer, FRAME_SIZE, 
			               0, (struct sockaddr *)&client, 
			               (socklen_t *)&sizeClient);
			if (ret == -1) {
				perror("error in recvfrom()");
				exit(EXIT_FAILURE);
			}

			/// Отправить end-poing клиента и его кадр в очередь сообщений
			/// с которым будет разбираться один из потоков (под-сервер): 
			struct messageInMqUdp message;
			strcpy(message.protocol, "UDP\0");
			message.client = client;
			strcpy(message.frame, inBuffer);
			message.fdSockUDP = fdSockUDP;
			ret = mq_send(mqDescr, (char *)&message, 
				          sizeof(struct messageInMqUdp), 5);
			if (ret == -1) {
			    perror("error in mq_send(), UDP for main thread");
			    exit(EXIT_FAILURE);
			}
		} //конец if (fds[0].revents & POLLIN)

		/// если получен кадр по протоколу TCP:
		if (fds[1].revents & POLLIN) {
			/// Шаг 4. Ожидание входящих подключений:
			fdDataSockTCP = accept(fdConnectSockTCP, NULL, NULL);
			if (fdDataSockTCP == -1) {
				perror("error in accept(), TCP");
				exit(EXIT_FAILURE);
			}
			printf("\nsuccessful accept(), TCP protocol\n");
			
			/// отослать сообщение через MQ:
			struct messageInMqTcp message;
			strcpy(message.protocol, "TCP\0");
			message.fdDataSockTCP = fdDataSockTCP;
			ret = mq_send(mqDescr, (char *)&message, sizeof(message), 5);
			if (ret == -1) {
			    perror("error in mq_send(), TCP for main thread");
			    exit(EXIT_FAILURE);
			}
		}
	} //конец while(1)
	
	exit(EXIT_SUCCESS);
}
