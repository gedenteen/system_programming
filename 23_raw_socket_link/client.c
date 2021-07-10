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

void sendingMessages(struct sockaddr_ll server, int fdSocket)
{
	socklen_t sockaddrLen = sizeof(struct sockaddr_in);
	int ret;

	/// Считывание сообщения из stdin:
	printf("enter your message: ");
	char *message = malloc((PACKET_SIZE - sockaddrLen) * sizeof(char));
	memset(message, 0, PACKET_SIZE - sockaddrLen);
	fgets(message, PACKET_SIZE - sockaddrLen, stdin);

	/// Создание пакета:
	char *packet = malloc(PACKET_SIZE * sizeof(char));
	memmove(packet + sizeof(struct ether_header) +
	        + sizeof(struct iphdr) + sizeof(struct udphdr),
	        message, strlen(message) - 1);
	//-1 чтобы убрать '\n'

	/// zapolnenie Ethernet-zagalovka:
	unsigned char dest[6] = {0x08, 0x00, 0x27, 0x95, 0xbb, 0xaa};
	unsigned char source[6] = {0x08, 0x00, 0x27, 0x96, 0xaf, 0xb8};
	struct ether_header *etherHeader;
	etherHeader = (struct ether_header *) packet;
	for (int i = 0; i < 6; i++) {
		etherHeader->ether_shost[i] = source[i];
		etherHeader->ether_dhost[i] = dest[i];
	}
	etherHeader->ether_type = htons(ETHERTYPE_IP);
	//v etom zagalovke ne nushno perevodit' v Big-endian

	/// Заполнение IP-заголовка:
	struct iphdr *ipHeader;
	ipHeader = (struct iphdr *) (packet + sizeof(struct ether_header));
	ipHeader->ihl = 5; //длина заголовка в 4-байтных словах
	ipHeader->version = 4; //версия IP протокола
	ipHeader->tos = 0; //тип сервиса, приоритет важности информации
	ipHeader->id = htonl(54321); //Id of this packet
	ipHeader->frag_off = 0; //флаги и смещение в 8-битных словах (если пакет фрагментирован)
	ipHeader->ttl = 255; //time to live, через сколько узлов может пройти пакет
	ipHeader->protocol = IPPROTO_UDP; //протокол транспортного уровня
	ipHeader->saddr = inet_addr(CLIENT_IP);//INADDR_ANY; //адрес-источник 
	ipHeader->daddr = inet_addr(SERVER_IP);//server.sin_addr.s_addr; //адрес-куда-доставить
	ipHeader->tot_len = htons(ipHeader->ihl * 4 +
	                          sizeof(struct udphdr) +
	                          strlen(message) - 1);
	ipHeader->check = 0;
	ipHeader->check = csum((unsigned short *) ipHeader, 
	                       ipHeader->ihl * 4);
	//teper' nado vyshityvat' chek-summu (predvaritelno ee zanuliv)

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

	/// Обработка всех UDP-пакетов в системе:
	while(1) {
		memset(packet, 0, PACKET_SIZE);
		ret = recvfrom(fdSocket, packet, PACKET_SIZE, 0, 
			           (struct sockaddr *)&server, &sockaddrLen);
		if (ret < 0) {
			perror("error in recvfrom()");
			exit(EXIT_FAILURE);
		}

		etherHeader = (struct ether_header *) packet;
		short int correctMac = 1;
		for (int i = 0; i < 6; i++) {
			if (etherHeader->ether_dhost[i] != source[i]) {
				correctMac = 0;
				break;
			}
		}
		if (correctMac == 0) 
			continue; //togda posmotret' sleduyshii packet
		
		ipHeader = (struct iphdr *) 
		           (packet + sizeof(struct ether_header));
		struct in_addr addr;
		addr.s_addr = ipHeader->daddr;
		
		if (strcmp(CLIENT_IP, inet_ntoa(addr)) != 0)
			continue; //togda posmotret' sleduyshii packet        
		           
		udpHeader = (struct udphdr *) 
		            (packet + sizeof(struct ether_header) +
		             ipHeader->ihl * 4);
		           
		if (ntohs(udpHeader->dest) != myPort) 
			continue; //togda posmotret' sleduyshii packet
			
		printf("\nreceived packet:\n");
		printf("destination MAC-address = ");
		for (int i = 0; i < 6; i++)
			printf("%0X ", (int) etherHeader->ether_dhost[i]);
		printf("\n");
		printf("destination IP-address = %s\n", inet_ntoa(addr)); 
		printf("destination port = %d\n", ntohs(udpHeader->dest)); 
		
			printf("received packet = %d bytes\n", ret);  
			printf("message: %s\n\n", (packet + sizeof(struct ether_header) + ipHeader->ihl * 4 + sizeof(struct udphdr)));

			break; //перестать обрабатывать UDP-пакеты
		
	}

	free(message), free(packet);
}

int main(void)
{
	/// End-point сервера:
	struct sockaddr_ll server;
	memset(&server, 0, sizeof(server));
	server.sll_family = AF_PACKET;
	server.sll_ifindex = if_nametoindex("enp0s3"); //index setevoy karti
	server.sll_halen = 6; //dlina MAC-address v bytes
	unsigned char mac_dest[6] = {0x08, 0x00, 0x27, 0x95, 0xbb, 0xaa};
	memmove((void *)(server.sll_addr), (void *)mac_dest, 6); //MAC-address
	
	/// Сокет:
	int fdSocket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	//ETH_P_ALL - vse protocoly Ethernet
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
