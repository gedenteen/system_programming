#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> //для sockaddr_un; un от слова UNIX
#include <string.h>
#include <stdlib.h>

#define SOCKET_FILENAME "/tmp/mySocket.socket"
#define BUFFER_SIZE 256

int main(int argc, char *argv[]) 
{
	struct sockaddr_un server;
	server.sun_family = AF_LOCAL;
	strncpy(server.sun_path, SOCKET_FILENAME, sizeof(server.sun_path) - 1);
	
	/// Шаг 1. Создать файловый дескриптор сокета
	/// (локальное IPC, потоковый протокол TCP, протокол по умолчанию):
	int fdDataSocket = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (fdDataSocket == -1) {
		perror("error in socket()");
		exit(EXIT_FAILURE);
	}
	
	/// Шаг 2. Соединиться 
	int ret = connect(fdDataSocket, (const struct sockaddr *) &server,
	                  sizeof(struct sockaddr_un)); 
	if (ret == -1) {
		perror("error in connect()");
		exit(EXIT_FAILURE);
	}
	
	/// 
    char buffer[BUFFER_SIZE];
	for (int i = 1; i < argc; i++) {
		ret = write(fdDataSocket, argv[i], strlen(argv[i]) + 1);
		if (ret == -1) {
			perror("error in write()");
			exit(EXIT_FAILURE);
		}
		
		ret = read(fdDataSocket, buffer, BUFFER_SIZE);
		if (ret == -1) {
			perror("error in read()");
			exit(EXIT_FAILURE);
		}
		
		printf("    sent: %s\n", argv[i]);
		printf("received: %s\n\n", buffer);
	}
	
	strcpy(buffer, "END");
	ret = write(fdDataSocket, buffer, BUFFER_SIZE);
	if (ret == -1) {
		perror("error in write()");
		exit(EXIT_FAILURE);
	}
	
	
	
	/// Шаг 4. 
	//fdNewSocket = acept(fdSocket, , length());
	
	close(fdDataSocket);
	exit(EXIT_SUCCESS);
}
