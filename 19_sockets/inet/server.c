#include "header.h"

int main(void) 
{
	/// заполнить структуру с адресом для сокета:
	struct sockaddr_in server;
	server.sin_family = AF_INET; //sun = socket UNIX, sin = socket inet
	server.sin_port = htons(2345); //перевести в big endian
	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	
	/// Шаг 1. Создать файловый дескриптор сокета
	int fdSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (fdSocket == -1) {
		perror("error in socket()");
		exit(EXIT_FAILURE);
	}
	
	/// Шаг 2. Связывание сокета и адреса:
	int ret = bind(fdSocket, (const struct sockaddr *) &server, 
	               sizeof(struct sockaddr_in));
	if (ret == -1) {
		perror("error in bind()");
		exit(EXIT_FAILURE);
	}
	
	/// получить 2 сообщения:
	char buffer[BUFFER_SIZE];
	for (int i = 0; i < 2; i++) {
		ret = recvfrom(fdSocket, buffer, BUFFER_SIZE, 0, NULL, NULL);
		printf("received: %s\n", buffer);
	}
	
	/// удалить сокет:
	close(fdSocket);
	exit(EXIT_SUCCESS);
}
