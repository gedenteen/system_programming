#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <pthread.h>
#include "libmy_strerror.h"

static void * threadFunc(void *arg) {
	char *str;
	printf("Other thread about to call strerror()\n");
	str = my_strerror(8);
	printf("Other thread: str (%p) = %s\n", str, str);
	return NULL;
}

int main(int argc, char *argv[]) {
	pthread_t t;
	int s;
	char *str;
	
	str = my_strerror(1);
	printf("Main thread has called strerror()\n");
	
	s = pthread_create(&t, NULL, threadFunc, NULL);
	if (s != 0) {
		fprintf(stderr, "error: pthread_create\n");
		exit(1);
	}
		
	s = pthread_join(t, NULL);
	if (s != 0) {
		fprintf(stderr, "error: pthread_join\n");
		exit(1);
	}
		
	printf("Main thread: str (%p) = %s\n", str, str);
	//char *new_str = my_strerror(3);
	//printf("Main thread: old str = %s\n\t new str = %s\n", str, new_str);
	exit(EXIT_SUCCESS);
}
