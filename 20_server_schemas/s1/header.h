#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <string.h> 
#include <sys/types.h> //для сокетов
#include <sys/socket.h> //для сокетов
#include <sys/un.h> //для sockaddr_un; un от слова UNIX
#include <stdlib.h> //exit()
#include <unistd.h> //для работы с файлами: close(), unlink()

#define SOCKET_FILENAME "/tmp/mySocket.socket"
#define BUFFER_SIZE 256

#endif
