#include "header.h"

int main() 
{
	/// заполнить структуру с адресом для сокета:
	struct sockaddr_in server;
	server.sin_family = AF_INET; //sun = socket UNIX, sin = socket inet
	server.sin_port = htons(PORT); //перевести в big endian
	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	
	/// Шаг 1. Создать файловый дескриптор сокета
	int fdDataSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (fdDataSocket == -1) {
		perror("error in socket()");
		exit(EXIT_FAILURE);
	}
	
	while (1) {
    	char buffer[FRAME_SIZE] = {0};
		printf("enter your message for the server (\"END\" to exit):\n");
		fgets(buffer, FRAME_SIZE, stdin);

		/// если получена команда для завершения общения: 
		if (strncmp(buffer, "END", 3) == 0)
			break;
		
		/// отправка сообщения (-1 чтобы убрать перевод строки):
		int ret = sendto(fdDataSocket, buffer, strlen(buffer) - 1, 0,
		                 (struct sockaddr *)&server, 
		                 sizeof(server));
		if (ret == -1) {
			perror("error in sendto()");
			exit(EXIT_FAILURE);
		}
		printf("    sent: %s", buffer);

		///получение сообщения:
		ret = recv(fdDataSocket, buffer, FRAME_SIZE, 0);
		if (ret == -1) {
			perror("error in recv()");
			exit(EXIT_FAILURE);
		}
		printf("received: %s\n\n", buffer);
	}
	
	close(fdDataSocket);
	exit(EXIT_SUCCESS);
}
