#include "header.h"

struct UdpHeader {
	unsigned short sourcePort;
	unsigned short destPort;
    unsigned short udpLen;
    unsigned short udpSum;
};

int main(void) 
{
	/// заполнить структуру с адресом для сокета:
	struct sockaddr_in server;
	server.sin_family = AF_INET; //sun = socket UNIX, sin = socket inet
	server.sin_port = htons(SERVER_PORT); //перевести в big endian
	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	
	/// Шаг 1. Создать файловый дескриптор сокета
	int fdSocket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (fdSocket == -1) {
		perror("error in socket()");
		exit(EXIT_FAILURE);
	}
	printf("RAW socket is created\n");

	//const int my

	///
	struct UdpHeader udpHeader; 
	const int totalSize = sizeof(udpHeader) + MESSAGE_SIZE * sizeof(char);
    udpHeader.sourcePort = htons(7654);
    udpHeader.destPort = htons(SERVER_PORT);
    udpHeader.udpLen = htons(totalSize);
    udpHeader.udpSum = 0;
    printf("UDP header is created\n");
	
	/// отправка сообщений:
	while (1) {
		/// считать сообщение, вводимое пользователем:
    	char message[MESSAGE_SIZE] = {0};
		printf("enter your message for the server (\"END\" to exit):\n");
		fgets(message, MESSAGE_SIZE, stdin);

		/// выделить память для отправки кадра (или не кадра):
		char *bufferToSend = malloc(totalSize + 1); //буфер для отправки на сервер
		char *ptrToBuffer = bufferToSend; //указатель на этот буфер

		/// в начале UDP-заголовок, потому что в RAW заголовок для 
		/// канального уровня не заполняется:
        memcpy(ptrToBuffer, &udpHeader, sizeof(udpHeader));
        ptrToBuffer += sizeof(udpHeader);

        /// теперь само сообщение, т.е. payload:
        memcpy(ptrToBuffer, message, MESSAGE_SIZE * sizeof(char));
		
		/// отправка сообщения (-1 чтобы убрать перевод строки):
		int ret = sendto(fdSocket, bufferToSend, totalSize, 0,
		                 (struct sockaddr *)&server, 
		                 sizeof(server));
		if (ret == -1) {
			perror("error in sendto()");
			exit(EXIT_FAILURE);
		}
		printf("    sent: %s", message);

		/// если получена команда для завершения общения: 
		if (strncmp(message, "END", 3) == 0)
			break;

		///получение сообщения:
		ret = recv(fdSocket, message, MESSAGE_SIZE, 0);
		if (ret == -1) {
			perror("error in recv()");
			exit(EXIT_FAILURE);
		}
		printf("received: %s\n\n", message);
	}
	
	close(fdSocket);
	exit(EXIT_SUCCESS);
}
