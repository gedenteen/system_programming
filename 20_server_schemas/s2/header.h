#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <string.h> 
#include <sys/types.h> //для сокетов
#include <sys/socket.h> //для сокетов
#include <netinet/in.h> //для sockaddr_in
#include <stdlib.h> //exit()
#include <unistd.h> //для работы с файлами: close(), unlink()

#define BUFFER_SIZE 256
#define CNT_THREADS 10

#define SERVER_QUEUE_NAME "/sp-example-server"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

#endif
