#include "header.h"

int main(void) 
{
	/// заполнить структуру с адресом для сокета:
	struct sockaddr_in server;
	server.sin_family = AF_INET; //sun = socket UNIX, sin = socket inet
	server.sin_port = htons(SERVER_PORT); //перевести в big endian
	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	
	/// Шаг 1. Создать файловый дескриптор сокета
	int fdSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (fdSocket == -1) {
		perror("error in socket()");
		exit(EXIT_FAILURE);
	}
	
	/// Шаг 2. Связывание сокета и адреса:
	int ret = bind(fdSocket, (const struct sockaddr *)&server, 
	               sizeof(struct sockaddr_in));
	if (ret == -1) {
		perror("error in bind()");
		exit(EXIT_FAILURE);
	}

	/// цикл обработки сообщений:
	for (int i = 0; ; i++) {
		/// end-point клиента:
		struct sockaddr_in client;
		int sizeClient = sizeof(client);

		/// Шаг 3. Ожидание входящих сообщений:
		char inBuffer[MESSAGE_SIZE] = {0};
		ret = recvfrom(fdSocket, inBuffer, MESSAGE_SIZE, 
		               0, (struct sockaddr *)&client, 
		               (socklen_t *)&sizeClient);
		if (ret == -1) {
			perror("error in recvfrom()");
			exit(EXIT_FAILURE);
		}

		/// лог:
		printf("received (port=%d): %s\n", 
			   ntohs(client.sin_port), inBuffer);

		/// отправить end-poing клиента и его пакет в очередь сообщений
		/// с которым будет разбираться один из потоков (под-сервер): 
		strcat(inBuffer, " echo-reply");
		int ret = sendto(fdSocket, inBuffer, MESSAGE_SIZE, 
	                     0, (struct sockaddr*)&client, 
	                     sizeClient);
		if (ret == -1) {
			perror("error in sendto()");
			exit(EXIT_FAILURE);
		}
	}
	
	/// удалить сокет:
	close(fdSocket);
	exit(EXIT_SUCCESS);
}
