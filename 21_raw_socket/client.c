#include "header.h"

void sendingMessages(struct sockaddr_in server, int fdSocket)
{
	socklen_t sockaddrLen = sizeof(struct sockaddr_in);
	int ret;

	/// Считывание сообщения из stdin:
	printf("enter your message: ");
	char *message = malloc((PACKET_SIZE - sockaddrLen) * sizeof(char));
	memset(message, 0, PACKET_SIZE - sockaddrLen);
	fgets(message, PACKET_SIZE - sockaddrLen, stdin);

	/// Создание пакета с UDP-заголовком:
	char *packet = malloc(PACKET_SIZE * sizeof(char));
	memmove(packet + 8, message, strlen(message) - 1);
	//-1 чтобы убрать '\n'

	/// Заполнение UDP-заголовка:
	const int myPort = 7654;
	struct udphdr *udpHeader;
	udpHeader = (struct udphdr *)packet;
	udpHeader->source = htons(myPort);
	udpHeader->dest = htons(SERVER_PORT);
	udpHeader->check = 0;
	udpHeader->len = htons(strlen(message) - 1 + sizeof(struct udphdr));
	//-1 чтобы убрать '\n'

	/// Отправка пакета:
	ret = sendto(fdSocket, packet, PACKET_SIZE, 0, 
		         (struct sockaddr *)&server, sockaddrLen);
	if (ret < 0) {
		perror("error in sendto()");
		exit(EXIT_FAILURE);
	}
	printf("sended to server %d bytes\n", ret);

	/// Обработка всех UDP-пакетов в системе:
	while(1) {
		memset(packet, 0, PACKET_SIZE);
		ret = recvfrom(fdSocket, packet, PACKET_SIZE, 0, 
			           (struct sockaddr *)&server, &sockaddrLen);
		if (ret < 0) {
			perror("error in recvfrom()");
			exit(EXIT_FAILURE);
		}

		struct iphdr *ipHeader = (struct iphdr *)packet;
		udpHeader = (struct udphdr *) (packet + sizeof(struct iphdr));

		/// Если в пакете указан придуманный порт выше (7654):
		if (ntohs(udpHeader->dest) == myPort) {
			struct in_addr saddr;
			saddr.s_addr = ipHeader->saddr;
			printf("server IP = %s, source port = %d, destination port = %d\n", 
				   inet_ntoa(saddr), 
				   ntohs(udpHeader->source), ntohs(udpHeader->dest));
			printf("received packet = %d bytes\n", ret);  
			printf("message: %s\n\n", packet + sizeof(struct iphdr) 
				   + sizeof(struct udphdr));

			break; //перестать обрабатывать UDP-пакеты
		}
	}

	free(message), free(packet);
}

int main(void)
{
	/// End-point сервера:
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);
	server.sin_addr.s_addr = INADDR_ANY;

	/// Сокет:
	int fdSocket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (fdSocket < 0) {
		perror("error in socket()");
		exit(EXIT_FAILURE);
	}

	while (1) {
		sendingMessages(server, fdSocket);
	}

	close(fdSocket);
	return 0;
}