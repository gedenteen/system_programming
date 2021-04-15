#include <stdio.h>
#include <sys/types.h>
#include <unistd.h> 
#include <wait.h>
#include <stdlib.h>
#include <string.h> 


const int BSIZE = 100;

int process_params(int argc, char *argv[], int arg_ind, char buf[])
{
	if (strcmp("my_grep", argv[arg_ind]) == 0) { //если первый парамер == "my_grep"
		arg_ind++; //тогда рассмотреть следующий параметр
		char line[BSIZE]; //строка в buf, строки разделяются '\n'
		int line_ind = 0; //индекс для line
		for (int i = 0; i < BSIZE; i++) { //цикл по всей длине буфера, который получен при exec()
			if (buf[i] != '\n') { //если не новая строка в буфере 
			    line[line_ind] = buf[i]; //тогда дописать текущую строку
			    line_ind++;
			}
			else { 
			    if (strstr(line, argv[arg_ind]) != NULL) //сравнить строку с параметром (словом для поиска)
			        printf("%s\n", line);
			    memset(line, 0, sizeof(line)); //затереть данные в строке
			    line_ind = 0;
			}
		}
	}
    else {
    	fprintf(stderr, "unknown parametr %s\n", argv[arg_ind]);
    	return -1;
    }
    
    return 0;
}

//командный интерпретатор - запуск бинарных файлов 
int main(int argc, char *argv[]) {
	int fildes[2];
	if (pipe(fildes)) { //создать неименновый канал
		printf("error in pipe()\n");
		exit(1);
	}

	while (1) {
		printf("\nenter name of binary file (type \"q\" for exit):\n");
		char *binary_file; 
		scanf("%ms", &binary_file); //считать строку с выделением памяти
		
		if (strcmp("q", binary_file) == 0) {
			//close(fildes[1]); close(fildes[1]);
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
			
			
			pid_t pid1 = fork();
			switch (pid1) {
				case -1: {
					printf("error in fork()\n");
					exit(EXIT_FAILURE);
				}
				case 0: {
					
					//close(fildes[1]); //закрыть дескриптор на запись
					//printf("execl = %d\n", execl(binary_file, binary_file, NULL));
					//printf("child, before change stdiin\n");
					
					int reserve_fd;
					if ((reserve_fd = dup( 1 )) == -1) {
						printf("error in dup\n");
					}
					
					//printf("reserve_fd = %d\n", reserve_fd);
					if (dup2(fildes[1], 1) == -1) {
						fprintf(stderr, "error in dup2\n");
					}				
					/////////////
					if (execl(binary_file, binary_file, NULL) == -1) { //exec() замещает текущий процесс
						fprintf(stderr, "unable to exec\n");
						exit(EXIT_FAILURE);
					}				
					/////////////
					if (dup2(reserve_fd, 1) == -1) {
						fprintf(stderr, "error in dup2(reserve_fd, 1)\n");
					}
				}
				default: {
					int status;
					waitpid(pid1, &status, 0);
					//printf("completed process status = %d\n", status);
					char buf[BSIZE];
					ssize_t nbytes;
						///////////////man 7 pipe
						//write(fildes[1], "\n", 1);
					nbytes = read(fildes[0], buf, BSIZE);
					//printf("buf = %s\n", buf);
					//printf("argv[1] = %s\n", argv[1]);
					printf("\n");
					process_params(argc, argv, 1, buf);//
					printf("\n");
				}
			}
			//close(fildes[0]);
			exit(EXIT_SUCCESS);
		}
		
		free(binary_file);
	}
	
	return -1;
}
