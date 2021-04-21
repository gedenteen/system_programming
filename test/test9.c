#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

int main() {
	int fildes[2];
	if (pipe(fildes)) { //создать неименновый канал
		printf("error in pipe()\n");
		exit(1);
	}
	
	const int BSIZE = 100;
    char buf[BSIZE];
    ssize_t nbytes;


	switch (fork()) { //создать дочерний процесс, который прочитает строку через канал
		case -1: {
			printf("error in fork()\n");
			exit(1);
		}
		case 0: { //потомок
			close(fildes[1]); //закрыть дескриптор на запись
			nbytes = read(fildes[0], buf, BSIZE);
			printf("%s from child procces\n", buf);
			close(fildes[0]);
			exit(EXIT_SUCCESS);
		}
		default: { //родитель
			close(fildes[0]); //закрыть дескриптор на чтение
			write(fildes[1], "Hello world\n", 12);
			close(fildes[1]);
			exit(EXIT_SUCCESS);
		}
	}
	
	exit(1);
}
