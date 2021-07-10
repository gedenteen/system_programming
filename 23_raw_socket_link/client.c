#include "header.h"

/// вычисление чек-суммы
/// код взят из https://www.binarytides.com/raw-sockets-c-code-linux/
unsigned short csum(unsigned short *ptr,int nbytes)
{
	register long sum = 0;
	unsigned short oddbyte;
	register short answer;

	while(nbytes>1) {
		sum+=*ptr++;
		nbytes-=2;
	}
	if(nbytes==1) {
		oddbyte=0;
		*((u_char*)&oddbyte)=*(u_char*)ptr;
		sum+=oddbyte;
	}

	sum = (sum>>16)+(sum & 0xffff);
	sum = sum + (sum>>16);
	answer=(short)~sum;
	
	return(answer);
}

void sendingMessages(struct sockaddr_ll server, int fdSocket,
	                 unsigned char macSource[6], unsigned char macDest[6])
{
	socklen_t sockaddrLen = sizeof(struct sockaddr_ll);
	int ret;

	/// Считывание сообщения из stdin:
	printf("enter your message: ");
	char *message = malloc((PACKET_SIZE - sockaddrLen) * sizeof(char));
	memset(message, 0, PACKET_SIZE - sockaddrLen);
	fgets(message, PACKET_SIZE - sockaddrLen, stdin);

	/// Создание пакета:
	char *packet = malloc(PACKET_SIZE * sizeof(char));

	/// Копирование сообщения в пакет:
	memmove(packet + sizeof(struct ether_header) +
	        + sizeof(struct iphdr) + sizeof(struct udphdr),
	        message, strlen(message) - 1);
	//-1 чтобы убрать '\n'

	/// Заполнение Ethernet-заголовка:
	struct ether_header *etherHeader;
	etherHeader = (struct ether_header *) packet;
	for (int i = 0; i < 6; i++) {
		etherHeader->ether_shost[i] = macSource[i];
		etherHeader->ether_dhost[i] = macDest[i];
	}
	etherHeader->ether_type = htons(ETHERTYPE_IP);
	// не надо переводить в Big-endian

	/// Заполнение IP-заголовка:
	struct iphdr *ipHeader;
	ipHeader = (struct iphdr *) (packet + sizeof(struct ether_header));
	ipHeader->ihl = 5; //длина заголовка в 4-байтных словах
	ipHeader->version = 4; //версия IP протокола
	ipHeader->tos = 0; //тип сервиса, приоритет важности информации
	ipHeader->id = htonl(54321); //ID пакета
	ipHeader->frag_off = 0; //флаги и смещение в 8-битных словах (если пакет фрагментирован)
	ipHeader->ttl = 255; //time to live, через сколько узлов может пройти пакет
	ipHeader->protocol = IPPROTO_UDP; //протокол транспортного уровня
	ipHeader->saddr = inet_addr(CLIENT_IP); //адрес-источник 
	ipHeader->daddr = inet_addr(SERVER_IP); //адрес-куда-доставить
	ipHeader->tot_len = htons(ipHeader->ihl * 4 +
	                          sizeof(struct udphdr) +
	                          strlen(message) - 1);
	ipHeader->check = 0;
	ipHeader->check = csum((unsigned short *) ipHeader, 
	                       ipHeader->ihl * 4);
	//в этой реализации уже нужно считать чек-сумму (предварительно ее занулив)

	/// Заполнение UDP-заголовка:
	const int myPort = 7654;
	struct udphdr *udpHeader;
	udpHeader = (struct udphdr *) (packet + 
	            sizeof(struct ether_header) + sizeof(struct iphdr));
	udpHeader->source = htons(myPort);
	udpHeader->dest = htons(SERVER_PORT);
	udpHeader->check = 0;
	udpHeader->len = htons(strlen(message) - 1 + sizeof(struct udphdr));
	//-1 чтобы убрать '\n'

	/// Отправка пакета:
	sockaddrLen = sizeof(struct sockaddr_ll);
	ret = sendto(fdSocket, packet, PACKET_SIZE, 0, 
		     (struct sockaddr *)&server, sockaddrLen);
	if (ret < 0) {
		perror("error in sendto()");
		exit(EXIT_FAILURE);
	}
	printf("sended to server %d bytes\n", ret);

	/// Обработка всех пакетов в системе:
	while(1) {
		/// Получение пакета:
		memset(packet, 0, PACKET_SIZE);
		ret = recvfrom(fdSocket, packet, PACKET_SIZE, 0, 
			           (struct sockaddr *)&server, &sockaddrLen);
		if (ret < 0) {
			perror("error in recvfrom()");
			exit(EXIT_FAILURE);
		}

		/// Проверка Ethernet-заголовка:
		etherHeader = (struct ether_header *) packet;
		short int correctMac = 1;
		for (int i = 0; i < 6; i++) {
			if (etherHeader->ether_dhost[i] != macSource[i]) {
				correctMac = 0;
				break;
			}
		}
		if (correctMac == 0) 
			continue; //тогда посмотреть следующий пакет
		
		/// Проверка IP-заголовка:
		ipHeader = (struct iphdr *) 
		           (packet + sizeof(struct ether_header));
		struct in_addr addr;
		addr.s_addr = ipHeader->daddr;
		if (strcmp(CLIENT_IP, inet_ntoa(addr)) != 0)
			continue; //тогда посмотреть следующий пакет      
		        
		/// Проверка UDP-заголовка:      
		udpHeader = (struct udphdr *) 
		            (packet + sizeof(struct ether_header) +
		             ipHeader->ihl * 4);     
		if (ntohs(udpHeader->dest) != myPort) 
			continue; //тогда посмотреть следующий пакет
			
		/// Если получен пакет от сервера:	
		printf("\nreceived packet:\n");
		printf("destination MAC-address = ");
		for (int i = 0; i < 6; i++)
			printf("%02X ", (int) etherHeader->ether_dhost[i]);
		printf("\n");
		printf("destination IP-address = %s\n", inet_ntoa(addr)); 
		printf("destination port = %d\n", ntohs(udpHeader->dest)); 
		printf("received packet = %d bytes\n", ret);  
		printf("message: %s\n\n", (packet + sizeof(struct ether_header) + ipHeader->ihl * 4 + sizeof(struct udphdr)));

		break; //перестать обрабатывать пакеты
	}

	free(message), free(packet);
}

int main(void)
{
	/// MAC-адреса:
	unsigned char macSource[6] = {0x08, 0x00, 0x27, 0x96, 0xaf, 0xb8};
	unsigned char macDest[6] = {0x08, 0x00, 0x27, 0x95, 0xbb, 0xaa};

	/// End-point сервера:
	struct sockaddr_ll server;
	memset(&server, 0, sizeof(server));
	server.sll_family = AF_PACKET;
	server.sll_ifindex = if_nametoindex("enp0s3"); //индекс сетевой карты
	server.sll_halen = 6; //длина MAC-адреса в байтах
	memmove((void *)(server.sll_addr), (void *)macDest, 6); //MAC-адрес
	
	/// Сокет:
	int fdSocket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	//ETH_P_ALL - все протоколы Ethernet
	if (fdSocket < 0) {
		perror("error in socket()");
		exit(EXIT_FAILURE);
	}

	while (1) {
		sendingMessages(server, fdSocket, macSource, macDest);
	}

	close(fdSocket);
	return 0;
}
