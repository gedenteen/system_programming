#include <stdio.h>
#include <string.h> //get declaration of str
#include <pthread.h>
#include <stdlib.h>

static void * threadFunc(void *arg) {
	char *str;
	printf("Other thread about to call strerror()\n");
	str = strerror(2);
	printf("Other thread: str (%p) = %s\n", str, str);
	return NULL;
}

int main(int argc, char *argv[]) {
	pthread_t t;
	int s;
	char *str;
	
	str = strerror(1);
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
	char *new_str = strerror(7);
	printf("Main thread: old str = %s\n\t new str = %s\n", str, new_str);
	exit(EXIT_SUCCESS);
}
