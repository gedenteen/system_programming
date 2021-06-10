#include "header.h"

int main() 
{
	/// заполнить структуру с адресом для сокета:
	struct sockaddr_in server;
	server.sin_family = AF_INET; //sun = socket UNIX, sin = socket inet
	server.sin_port = htons(2345); //перевести в big endian
	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	
	/// Шаг 1. Создать файловый дескриптор сокета
	/// (локальное IPC, потоковый протокол TCP, протокол по умолчанию):
	int fdDataSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (fdDataSocket == -1) {
		perror("error in socket()");
		exit(EXIT_FAILURE);
	}
	
	/// Шаг 2. Соединиться к уже созданному сокету:
	int ret = connect(fdDataSocket, (const struct sockaddr *) &server,
	                  sizeof(struct sockaddr_in)); 
	if (ret == -1) {
		perror("error in connect()");
		exit(EXIT_FAILURE);
	}
	
	while (1) {
    	char buffer[BUFFER_SIZE] = {0};//char *buffer = malloc(sizeof(char) * BUFFER_SIZE);
		printf("enter your message for the server (\"END\" to exit):\n");
		fgets(buffer, BUFFER_SIZE, stdin);
		
		ret = send(fdDataSocket, buffer, strlen(buffer) - 1, 0);
		//-1 чтобы убрать перевод строки
		if (ret == -1) {
			perror("error in send()");
			exit(EXIT_FAILURE);
		}
		printf("    sent: %s", buffer);
		
		/// если получена команда для завершения общения: 
		if (strncmp(buffer, "END", 3) == 0)
			break;
		
		ret = recv(fdDataSocket, buffer, BUFFER_SIZE, 0);
		if (ret == -1) {
			perror("error in recv()");
			exit(EXIT_FAILURE);
		}
		printf("received: %s\n\n", buffer);
		//free(buffer);
	}
	
	close(fdDataSocket);
	exit(EXIT_SUCCESS);
}
