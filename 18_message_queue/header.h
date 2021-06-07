#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h> //для exit() 
#include <string.h> //для выделения подстрок
//#include <sys/types.h>
#include <unistd.h> //для getpid()
//#include <fcntl.h>
//#include <sys/stat.h>
#include <mqueue.h> //для очереди сообщений

#define SERVER_QUEUE_NAME "/sp-example-server"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10
#endif
