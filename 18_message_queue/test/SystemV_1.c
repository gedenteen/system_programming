#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define MESSAGE_SIZE 80 //при 12 был баг, который я не смог найти

struct mymsgbuf { //структура для сообщения
  long mtype; //приоритет сообщения (должен быть > 0)
  char mtext[MESSAGE_SIZE]; //сообщение
};

int main(int argc, char *argv[]) {
	/// создать уникальный ключ через имя файла и proj_id: 
	key_t mesQueueKey = ftok(".", 'm'); //при proj_id == 18 программа не работала
	if (mesQueueKey == -1) {
		perror("error in ftok()"); 
		exit(EXIT_FAILURE);
	}
	
	/// создать очередь или подключиться к ней (через ключ):
	int mesQueueId = msgget(mesQueueKey, IPC_CREAT | 0660); //0660 - для уникальности доступа?
	if (mesQueueId == -1) {
		perror("error in msgget()"); 
		exit(EXIT_FAILURE);
	}
	
	
	/// отправить сообщение для SystemV_2.exe:
	struct mymsgbuf myMessage;
	myMessage.mtype = 1; //задать приоритет
	strcpy(myMessage.mtext, "hello"); //записать сообщение
	if (msgsnd(mesQueueId, (struct mymsgbuf *)&myMessage, 
	           strlen(myMessage.mtext)+1, 0) == -1) {
		perror("error in msgsnd()");
		exit(EXIT_FAILURE);
	}
	
	/// получить сообщение от SystemV_2.exe:
	struct mymsgbuf notMyMessage;
	notMyMessage.mtype = 2;
	if (msgrcv(mesQueueId, &notMyMessage, MESSAGE_SIZE, 2, 0) == -1) {
		perror("error in msgrcv()");
		exit(EXIT_FAILURE);
	}
 	printf("message from SystemV_2:\ntype: %ld; text: %s\n", 
 		notMyMessage.mtype, notMyMessage.mtext);
 		
	/// удаление очереди:
 	if (msgctl(mesQueueId, IPC_RMID, 0) == -1) {
		perror("error in msgctl()");
		exit(EXIT_FAILURE);
 	}	
	
	exit(EXIT_SUCCESS);
}
