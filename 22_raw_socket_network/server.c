#include "header.h"

int main(void)
{
	/// End-point сервера:
	struct sockaddr_in server, client;
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);
	server.sin_addr.s_addr = INADDR_ANY;
	
	/// Создание сокета:
	int fdSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fdSocket < 0) {
		perror("error in socket()");
		exit(EXIT_FAILURE);
	}

	/// Привязка сокета к end-поинту:
	socklen_t len = sizeof(struct sockaddr_in);
	int ret = bind(fdSocket, (struct sockaddr *)&server, len);
	if (ret < 0) {
		perror("error in bind()");
		exit(EXIT_FAILURE);
	}

	while(1) {
		/// Получение пакета от клиента:
		printf("\nwaiting for packet...\n");
		memset(&client, 0, sizeof(struct sockaddr_in));
		len = sizeof(struct sockaddr_in);
		char packet[PACKET_SIZE] = {0};
		ret = recvfrom(fdSocket, packet, PACKET_SIZE, 0, 
		               (struct sockaddr *)&client, &len);
		if (ret < 0) {
			perror("error in recvfrom()");
			exit(EXIT_FAILURE);
		}

		/// Логи:
		printf("client ip = %s, port = %d\n", 
			   inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		printf("received message = %d bytes: %s\n", ret, packet);

		/// Модифицировать сообщение и отправить его обратно:
		strcat(packet, " echo-reply\0");
		ret = sendto(fdSocket, packet, strlen(packet) * sizeof(char),
		             0, (struct sockaddr *) &client, len);
		if (ret < 0) {
			perror("error in sendto()");
			exit(EXIT_FAILURE);
		}
		printf("sended echo-reply, size = %d bytes\n", ret);
	}

	return 0;
}