#include "header.h"

int main(int argc, char *argv[]) 
{
	/// заполнить структуру с адресом для сокета:
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(2345); //перевести в big endian
	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	
	/// Шаг 1. Создать файловый дескриптор сокета
	int fdSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (fdSocket == -1) {
		perror("error in socket()");
		exit(EXIT_FAILURE);
	}
	
	/// отправить 2 сообщения:
	char msg1[] = "hello\0";
	sendto(fdSocket, msg1, sizeof(msg1), 0,  
		(struct sockaddr *)&server, sizeof(struct sockaddr_in));
	
	connect(fdSocket, (struct sockaddr *)&server, 
		sizeof(struct sockaddr_in));
	char msg2[] = "bye\0";
	send(fdSocket, msg2, sizeof(msg2), 0);
	
	close(fdSocket);
	exit(EXIT_SUCCESS);
}
