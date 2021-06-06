#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <stdlib.h>

int main() {
	/// подключение к очереди из POSIX_1.exe
	mqd_t mqDescriptor; //дескриптор очереди сообщений
	mqDescriptor = mq_open("/sp-example-server", O_WRONLY);
	if (mqDescriptor == -1) {
		perror("error in mq_open()");
		exit(EXIT_FAILURE);
	}
	
	/// отправка сообщения:
	char text[] = "howdy";
	if (mq_send(mqDescriptor, text, strlen(text) + 1, 0) == -1){
		perror("error in mq_send()");
		return -1;
	}
	
	exit(EXIT_SUCCESS);
}
