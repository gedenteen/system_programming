#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <string.h> 
#include <sys/types.h> //для сокетов
#include <sys/socket.h> //для сокетов
#include <netinet/in.h> //для sockaddr_in
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <arpa/inet.h> //для inet_ntoa()
#include <stdlib.h> //exit()
#include <unistd.h> //для работы с файлами: close(), unlink()

#define SERVER_PORT 7777
#define PACKET_SIZE 199

#endif
