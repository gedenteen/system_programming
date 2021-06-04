#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define SERVER_QUEUE_NAME   "/sp-example-server"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

struct message_t {
	char mqClientName[30];
	char command[10];
	char text[MAX_MSG_SIZE];
};

int main (int argc, char **argv)
{
    mqd_t qd_server, qd_client;   // queue descriptors
    long token_number = 1; // next token to be given to client

    printf ("Server: Hello, World!\n");

    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Server: mq_open (server)");
        exit (1);
    }
    char in_buffer [MSG_BUFFER_SIZE];
    char out_buffer [MSG_BUFFER_SIZE];

	char** mqClientsTable = malloc(sizeof(char*));
	int countClients = 0;
	
    while (1) {
        // get the oldest message with highest priority
        if (mq_receive (qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Server: mq_receive");
            exit (1);
        }

		long bytesForCopy = strcspn(in_buffer, ";");
		char* substr1 = malloc (sizeof(char) * bytesForCopy);
		strncpy(substr1, in_buffer, bytesForCopy); 
		
        printf ("Server: message received = %s\n", in_buffer);
		printf("substr1 = %s\n", substr1);
		
		if (strcmp("join", substr1) == 0) {
			countClients++;
			mqClientsTable = realloc(mqClientsTable, sizeof(char*) * countClients);
			mqClientsTable[countClients - 1] = malloc(
				strlen(in_buffer - bytesForCopy) * sizeof(char));
			strcpy(mqClientsTable[countClients - 1], in_buffer + bytesForCopy + 1);
			printf("new client %s joined\n", mqClientsTable[countClients - 1]);
			continue;
		}
		
		char message[MAX_MSG_SIZE];
		strcpy(message, in_buffer + bytesForCopy + 1);

		printf("message = %s\n", message);

        // send reply message to client

        if ((qd_client = mq_open (substr1, O_WRONLY)) == 1) {
            perror ("Server: Not able to open client queue");
            continue;
        }

        sprintf (out_buffer, "%ld", token_number);

        if (mq_send (qd_client, out_buffer, strlen (out_buffer) + 1, 0) == -1) {
            perror ("Server: Not able to send message to client");
            continue;
        }

        printf ("Server: response sent to client.\n");
        token_number++;
    }
}
