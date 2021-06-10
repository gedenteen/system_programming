#include "header.h"
#include <pthread.h>

void *FuncForThread(void *param) 
{
	int *fdDataSocket = (int*)param;
	printf("fdData = %d\n", *fdDataSocket);

	/// обработка пакетов (кадров?) клиента:	
	char buffer[BUFFER_SIZE];
	while(1) {
		/// ожидание пакета с данными:
		int ret = recv(*fdDataSocket, buffer, BUFFER_SIZE, 0);
		if (ret == -1) {
			perror("error in recv()");
			exit(EXIT_FAILURE);
		} 
		printf("received: %s\n", buffer);
		
		/// проверка на то, что буфер завершается 0:
		//buffer[BUFFER_SIZE - 1] = 0;
		
		/// если получена команда для завершения общения: 
		if (strncmp(buffer, "END", BUFFER_SIZE) == 0) {
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
		printf("   sent: %s\n", buffer);
	}
}

int main(void) 
{
	//TODO
	/// удалить файл с указанным именем
	unlink(SOCKET_FILENAME);
	
	/// заполнить структуру с локальным адресом для сокета:
	struct sockaddr_un server;
	server.sun_family = AF_LOCAL;
	strncpy(server.sun_path, SOCKET_FILENAME, sizeof(server.sun_path) - 1);
	
	/// Шаг 1. Создать файловый дескриптор сокета
	/// (локальное IPC, потоковый протокол TCP, протокол по умолчанию):
	int fdConnectSocket = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (fdConnectSocket == -1) {
		perror("error in socket()");
		exit(EXIT_FAILURE);
	}
	
	/// Шаг 2. Связывание сокета и адреса:
	int ret = bind(fdConnectSocket, (const struct sockaddr *) &server, 
	              sizeof(struct sockaddr_un));
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
	
	/// цикл обработки подключений:
	char ch = 0; //переменная для getch() 
	while (1) { //((ch = getchar()) != 27) { //27 - это код клавиши Esc 
		/// Шаг 4. Ожидание входящих подключений:
		printf("accept...\n");
		int fdDataSocket = accept(fdConnectSocket, NULL, NULL);
		if (fdDataSocket == -1) {
			perror("error in accept()");
			exit(EXIT_FAILURE);
		}
		
		printf("fdData = %d\n", fdDataSocket);
		/// создать поток:
		void *param = (void *)&fdDataSocket;
		pthread_t thrId;
		if (pthread_create(&thrId, &thrAttr, FuncForThread, param)) {
			fprintf(stderr, "error: can't create pthread\n");
			exit(EXIT_FAILURE);
		}
	} //конец while(1)	
	
	/// удалить сокет:
	close(fdConnectSocket);
	unlink(SOCKET_FILENAME);
	
	exit(EXIT_SUCCESS);
}
