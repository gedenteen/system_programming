#include "header.h"
#include <pthread.h> 
//#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h> //для очереди сообщений


void *FuncForThread(void *param) 
{
	char *mqName = (char *)param; //взять имя из параметра
	mqd_t mqDescr;
	if ((mqDescr = mq_open(mqName, O_RDONLY)) == 1) {
	    perror("error in mq_open() for created thread");
	    exit(EXIT_FAILURE);
	}

	while (1) {
		/// получить сообщение через очередь (блокирующий вызов):
	    char inBuffer[MSG_BUFFER_SIZE] = {0};
	    if (mq_receive(mqDescr, inBuffer, MSG_BUFFER_SIZE, NULL) == -1) {
	        perror("error in mq_receive() for created thread");
	   		exit(EXIT_FAILURE);
	    }
	    
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
			
			/// проверка на то, что буфер завершается 0:
			//buffer[BUFFER_SIZE - 1] = 0;
			
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
	int fdConnectSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (fdConnectSocket == -1) {
		perror("error in socket()");
		exit(EXIT_FAILURE);
	}
	
	/// Шаг 2. Связывание сокета и адреса:
	int ret = bind(fdConnectSocket, (const struct sockaddr *) &server, 
	               sizeof(struct sockaddr_in));
	if (ret == -1) {
		perror("error in bind()");
		exit(EXIT_FAILURE);
	}
	
	/// Шаг 3. Ожидание запросов от клиентов:
	if (listen(fdConnectSocket, 1) == -1) {
		perror("error in listen()");
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
	
	/// переменные для потоков:
	pthread_t thrId[CNT_THREADS]; //дескрипторы потоков 
	int fdDataSocket[CNT_THREADS] = {0}; //файловые дескрипторы сокетов
	mqd_t mq[CNT_THREADS]; //дескрипторы очередей сообщений
	char **mqNames = malloc(sizeof(char *) * CNT_THREADS); 

	/// создать очереди сообщений и потоки:
	for (int i = 0; i < CNT_THREADS; i++) {
		/// создание очерди сообщений:
		mqNames[i] = malloc(sizeof(char) * 32);
   		sprintf(mqNames[i], "/sp-client-%d", i);
   		mq[i] = mq_open(mqNames[i], O_WRONLY | O_CREAT, QUEUE_PERMISSIONS, &mqAttr);
		if (mq[i] == -1) {
	        perror("error in mq_open() for main thread");
	        exit(EXIT_FAILURE);
	    }

	    /// создание потока:
		void *param = (void *)mqNames[i];
		if (pthread_create(&thrId[i], &thrAttr, FuncForThread, param)) {
			fprintf(stderr, "error: can't create pthread\n");
			exit(EXIT_FAILURE);
		}
	}
	
	/// цикл обработки подключений:
	for (int i = 0; ; i++) {
		/// i - номер потока, проверка на выход из макс-го кол-ва:
		if (i == CNT_THREADS)
			i = 0;

		/// Шаг 4. Ожидание входящих подключений:
		fdDataSocket[i] = accept(fdConnectSocket, NULL, NULL);
		if (fdDataSocket[i] == -1) {
			perror("error in accept()");
			exit(EXIT_FAILURE);
		}
		printf("successful accept()\n");
		
		/// отослать сообщение через mq:
		char outBuffer[8] = {0};
		sprintf(outBuffer, "%d", fdDataSocket[i]);
		if (mq_send(mq[i], outBuffer, strlen(outBuffer) + 1, 5) == -1) {
		    perror("error in mq_send() for main thread");
		    exit(EXIT_FAILURE);
		}

		/// вывод лога:
		printf("send fdDataSocket == %d to thread %d\n", 
		       fdDataSocket[i], i);
	}	
	
	/// удалить сокет:
	close(fdConnectSocket);
	
	exit(EXIT_SUCCESS);
}
