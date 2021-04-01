#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <pthread.h>

#define MAX_ERROR_LEN 256

static pthread_key_t gl_key;
static pthread_once_t gl_once = PTHREAD_ONCE_INIT;

//функция-деструктор:
static void destructor(void *buf) {
	free(buf);
}

//создание ключа, общего для всех потоков одного процесса:
static void make_key() {
	if (pthread_key_create(&gl_key, destructor) != 0) {
		fprintf(stderr, "error in pthread_key_create()\n");
		exit(1);
	}
}

//реинтерабельная функция:
char * my_strerror(int err) {
	int status;
	char *buf;
	
	status = pthread_once(&gl_once, make_key);
	if (status != 0) {
		fprintf(stderr, "error in pthread_once()\n");
		exit(1);
	}
	
	buf = pthread_getspecific(gl_key);
	if (buf == NULL) {
		buf = malloc(MAX_ERROR_LEN);
		if (buf == NULL) {
			fprintf(stderr, "error in malloc()\n");
			exit(1);
		}
		status = pthread_setspecific(gl_key, buf);
		if (status != 0) {
			fprintf(stderr, "error in pthread_setspecific()\n");
			exit(1);
		}
	}
	
	switch (err) {
		case 1:
			strcpy(buf, "segmentation fault\0");
			break;
		case 2:
			strcpy(buf, "invalid arguments\0");
			break;
		case 3:
			strcpy(buf, "file not found\0");
			break;
		case 4:
			strcpy(buf, "operation not permitted\0");
			break;
		case 5:
			strcpy(buf, "not enough memory\0");
			break;
		case 6:
			strcpy(buf, "interrupted system call\0");
			break;
		case 7:
			strcpy(buf, "input/output error\0");
			break;
		case 8:
			strcpy(buf, "arguments list too long\0");
			break;
		case 9:
			strcpy(buf, "arguments list too short\0");
			break;
		default:
			snprintf(buf, MAX_ERROR_LEN, "unknown error %d", err);
			break;
	}
	return buf;
}
