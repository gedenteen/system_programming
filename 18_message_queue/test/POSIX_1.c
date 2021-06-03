#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <stdlib.h>

#define MESSAGE_SIZE 256

int main() {
	/// создание аттрибутов для очереди сообщений:
	struct mq_attr mqAttributes; //атрибуты очереди сообщений
	mqAttributes.mq_maxmsg = 32; //максимальное число сообщений
	mqAttributes.mq_msgsize = MESSAGE_SIZE; //максимальный размер сообщения
	mqAttributes.mq_flags = 0; //флаг - 0 или O_NONBLOCK
    mqAttributes.mq_curmsgs = 0; //текущее сообщение

	/// создание очереди:
	mqd_t mqDescriptor; //дескриптор очереди сообщений
	mqDescriptor = mq_open("/sp-example-server", O_RDONLY | O_CREAT, 0660, &mqAttributes);
	//я не знаю почему, но с некоторыми именами серверов не работает
	if (mqDescriptor == -1) {
		perror("error in mq_open()");
		exit(EXIT_FAILURE);
	}
	
	/// получение сообщения от POSIX_2.exe:
	char in_buffer[MESSAGE_SIZE + 10];
	if (mq_receive(mqDescriptor, in_buffer, MESSAGE_SIZE + 10, NULL) == -1) {
        perror("error in mq_receive()");
		exit(EXIT_FAILURE);
	}
	printf("received message: %s\n", in_buffer); 
	
	
	/// закрытие очереди (не удаление):
	if (mq_close(mqDescriptor) == -1) {
		perror("error in mq_close()");
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);
}
