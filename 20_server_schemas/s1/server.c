#include "header.h"
#include <pthread.h>

void *FuncForThread(void *param) 
{
	int *fdDataSocket = (int*)param;

	/// обработка пакетов (кадров?) клиента:	
	while(1) {
    	char buffer[BUFFER_SIZE] = {0};//char *buffer = malloc(sizeof(char) * BUFFER_SIZE);
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
		//free(buffer);
	}
}

int main(void) 
{
/*
	/// заполнить структуру с локальным адресом для сокета:
	unlink(SOCKET_FILENAME);//удалить файл с указанным именем
	struct sockaddr_un server;
	server.sun_family = AF_LOCAL;
	strncpy(server.sun_path, SOCKET_FILENAME, sizeof(server.sun_path) - 1);*/
	
	/// заполнить структуру с адресом для сокета:
	unlink(SOCKET_FILENAME);//удалить файл с указанным именем
	struct sockaddr_in server;
	server.sin_family = AF_INET; //sun = socket UNIX, sin = socket inet
	server.sin_port = htons(2345); //перевести в big endian
	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	
	/// Шаг 1. Создать файловый дескриптор сокета
	/// (локальное IPC, потоковый протокол TCP, протокол по умолчанию):
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
	/// (дескриптор сокета, максимальное кол-во клиентов в очереди):
	if (listen(fdConnectSocket, 1) == -1) {
		perror("error in listen()");
		exit(EXIT_FAILURE);
	}
	
	/// получить стандартные значения атрибутов для потоков:
	pthread_attr_t thrAttr;
	pthread_attr_init(&thrAttr);
	
	///.............
	const int maxSockets = 100;
	pthread_t thrId[maxSockets];
	int fdDataSocket[maxSockets];
	
	/// цикл обработки подключений:
	for (int i = 0; i < maxSockets; i++) {
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
	unlink(SOCKET_FILENAME);
	
	exit(EXIT_SUCCESS);
}
