#include <stdio.h>
#include <sys/types.h>
#include <unistd.h> 
#include <wait.h>
#include <stdlib.h>
#include <string.h> 

//командный интерпретатор - запуск бинарных файлов 
int main() {
	while (1) {
		printf("\nenter name of binary file (type \"q\" for exit):\n");
		char *binary_file; 
		scanf("%ms", &binary_file); //считать строку с выделением памяти
		
		if (strcmp("q", binary_file) == 0) {
			free(binary_file);
			exit(0);
		}
	
		pid_t pid = fork();
		if (pid == -1) {
			fprintf(stderr, "unable to fork\n");
		}
		if (pid) {
			int status;
			waitpid(pid, &status, 0);
			printf("completed process status = %d\n", status); 
		}
		else {
			if (execl(binary_file, binary_file, NULL) == -1) {
				fprintf(stderr, "unable to exec\n");
				exit(EXIT_FAILURE);
			}
			else
				exit(0);
		}
		
		free(binary_file);
	}
	
	return -1;
}
