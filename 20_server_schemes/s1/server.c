#include "header.h"
#include <pthread.h>

void *FuncForThread(void *param) 
{
	int *fdDataSocket = (int*)param;

	/// обработка пакетов (кадров?) клиента:	
	while(1) {
    	char buffer[BUFFER_SIZE] = {0};
		/// ожидание пакета с данными:
		int ret = recv(*fdDataSocket, buffer, BUFFER_SIZE, 0);
		if (ret == -1) {
			perror("error in recv()");
			exit(EXIT_FAILURE);
		} 
		printf("\nreceived: %s\n", buffer);
		
		/// проверка на то, что буфер завершается 0:
		//buffer[BUFFER_SIZE - 1] = 0;
		
		/// если получена команда для завершения общения: 
		if (strncmp(buffer, "END", BUFFER_SIZE) == 0) {
			printf("close fdDataSocket == %d...\n", *fdDataSocket);
			close(*fdDataSocket);
			*fdDataSocket = -1; //обозначить, что переменная свободна
			                    //для использования
			pthread_exit(0);
		}
		
		/// иначе сделать эхо-ответ:
		strcat(buffer, " echo-reply"); //добавить строку в конец существующей
		ret = send(*fdDataSocket, buffer, BUFFER_SIZE, 0);
		if (ret == -1) {
			perror("error in send()");
			exit(EXIT_FAILURE);
		} 
		printf("    sent: %s\n", buffer);
	}
}

int main(void) 
{	
	/// заполнить структуру с адресом для сокета:
	struct sockaddr_in server;
	server.sin_family = AF_INET; //sun = socket UNIX, sin = socket inet
	server.sin_port = htons(2345); //перевести в big endian
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
	
	/// создать переменные для потоков, которые будут храниться в памяти:
	/// (если создавать их в цикле, то они удалятся)
	const int maxSockets = 100; 
	pthread_t thrId[maxSockets]; 
	int fdDataSocket[maxSockets];

	/// инициализровать -1, которые означают, что данная переменная 
	/// никем не используется:
	for (int i = 0; i < maxSockets; i++)
		fdDataSocket[i] = -1;
	
	/// цикл обработки подключений:
	for (int i = 0; i < maxSockets; i++) {
		/// если итерация дошла до 100, то начать заново:
		if (i == maxSockets) 
			i = 0;

		/// если данный ФД испольуется, то его не трогать, попробовать
		/// следующий:
		if (fdDataSocket[i] != -1) 
			continue;

		/// Шаг 4. Ожидание входящих подключений:
		printf("accept...\n");
		fdDataSocket[i] = accept(fdConnectSocket, NULL, NULL);
		if (fdDataSocket[i] == -1) {
			perror("error in accept()");
			exit(EXIT_FAILURE);
		}
		
		/// создать поток:
		void *param = (void *)&fdDataSocket[i];
		if (pthread_create(&thrId[i], &thrAttr, FuncForThread, param)) {
			fprintf(stderr, "error: can't create pthread\n");
			exit(EXIT_FAILURE);
		}
	}	
	
	/// удалить сокет:
	close(fdConnectSocket);
	
	exit(EXIT_SUCCESS);
}
