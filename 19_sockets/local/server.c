#include "header.h"

int main(void) 
{
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
	
	/// здесь можно сделать цикл, в котором будут обрабатываться 
	/// несколько клиентов 
	
	/// Шаг 4. Ожидание входящих подключений:
	int fdDataSocket = accept(fdConnectSocket, NULL, NULL);
	if (fdDataSocket == -1) {
		perror("error in accept()");
		exit(EXIT_FAILURE);
	}
		
	/// обработка пакетов (кадров?) клиента:	
	char buffer[BUFFER_SIZE];
	while(1) {
		/// ожидание пакета с данными:
		ret = recv(fdDataSocket, buffer, BUFFER_SIZE, 0);
		if (ret == -1) {
			perror("error in read()");
			exit(EXIT_FAILURE);
		} 
		
		/// проверка на то, что буфер завершается 0:
		//buffer[BUFFER_SIZE - 1] = 0;
		
		/// если получена команда для завершения общения: 
		if (strncmp(buffer, "END", BUFFER_SIZE) == 0) {
			close(fdDataSocket);
			break;
		}
		
		/// иначе сделать эхо-ответ:
		strcat(buffer, " echo-reply"); //добавить строку в конец существующей
		ret = send(fdDataSocket, buffer, BUFFER_SIZE, 0);
		if (ret == -1) {
			perror("error in write()");
			exit(EXIT_FAILURE);
		} 
	}
	
	/// удалить сокет:
	close(fdConnectSocket);
	unlink(SOCKET_FILENAME);
	
	exit(EXIT_SUCCESS);
}
