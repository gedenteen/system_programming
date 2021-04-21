#include <stdio.h>
#include <sys/types.h>
#include <unistd.h> 
#include <wait.h>
#include <stdlib.h>
#include <string.h> 

#define STRING_LEN 256

int process_params(int argc, char *argv[], int arg_ind, char buf[])
{
	for ( ; arg_ind < argc; arg_ind++) {
		if (strcmp("my_grep", argv[arg_ind]) == 0) { //если первый парамер == "my_grep"
			arg_ind++; //тогда рассмотреть следующий параметр
			char line[STRING_LEN]; //строка в buf, строки разделяются '\n'
			int line_ind = 0; //индекс для line
			for (int i = 0; i < STRING_LEN; i++) { //цикл по всей длине буфера, который получен при exec()
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
    }
    
    return 0;
}

int highlight_words(char *string, int *words_cnt, char **words_arr[]) 
{ //функция для разделения строки, символ-разделить = ' '
	if (string[1] == '\n') { //если строка состоит из одного слова без пробелов
		*words_cnt = 1;
		*words_arr = realloc(*words_arr, sizeof(char*));
		(*words_arr)[0] = malloc(sizeof(char) * 2);
		(*words_arr)[0][0] = string[0];
		(*words_arr)[0][1] = '\0';
		return 0;
	}
	int i_beg = 0, i_end = -1; //индексы в строке, которые отвечают за начало и конец слова
	for (int i = 1; i < STRING_LEN; i++) { //цикл по длине строки
		if ((string[i] == ' ' || string[i] == '\n') && string[i - 1] != ' ') {
			i_end = i - 1; //индекс конца слова
			(*words_cnt)++; //увеличить переменную, отвечающую за кол-во слов
			//printf("i_beg = %2d, i_end = %2d, words_cnt = %2d\n\t", i_beg, i_end, *words_cnt); //отладка
			*words_arr = realloc(*words_arr, sizeof(char*) * (*words_cnt)); //изменить размер массива words_arr
			(*words_arr)[*words_cnt - 1] = malloc((i_end - i_beg + 2) * sizeof(char)); //выделить память под новую строку
			// +2 для '\0' и потому что 1 - 0 = 1, а длина = 2
			for (int j = 0; j <= i_end - i_beg; j++) { //скопировать слово по 1-му символу 
				(*words_arr)[*words_cnt - 1][j] = string[i_beg + j];
			}
			(*words_arr)[*words_cnt - 1][i_end - i_beg + 1] = '\0'; //добавить символ конца строки
			//printf("words_arr[words_cnt - 1] = %s.\n\n", (*words_arr)[*words_cnt - 1]); //отладка
			i_beg = i + 1; //задать начало для следующего слова
		}
		if (string[i] == '\n') { //если конец строки, то все слова найдены
			return 0;	
		}
	}
	fprintf(stderr, "error in highlight_words(), maybe not found '\\n' \n");
	return -1;
}
	
//командный интерпретатор - запуск бинарных файлов 
int main(int argc, char *argv[]) {
	int fildes[2];
	if (pipe(fildes)) { //создать неименновый канал
		printf("error in pipe()\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		printf("\nenter name of binary file (type \"q\" for exit):\n");
		char *input_string = (char*) malloc (STRING_LEN * sizeof(char)); 
		fgets(input_string, STRING_LEN, stdin); //считать строку, введенную пользователем
		int words_cnt = 0;
		char **words_arr = (char**) malloc(sizeof(char*)); //без маллока здесь ошибка при реаллоке в highlight_words()
		//free(words_arr[0]); //очистить память, так как может возникнуть ошибка при вызове "q" после любой другой программы
		highlight_words(input_string, &words_cnt, &words_arr); //выделить слова из строки
		
		if (strcmp("q", words_arr[0]) == 0) { //проверка на завершение программы
			//close(fildes[1]); close(fildes[1]);
			free(input_string);
			exit(EXIT_SUCCESS);
		}
	
		pid_t pid = fork(); //создать дочерний процесс
		if (pid == -1) {
			fprintf(stderr, "unable to fork\n");
		}
		if (pid) { //если это родительский процесс, то дождать выполнения потомка
			int status;
			waitpid(pid, &status, 0);
			printf("\tcompleted process status = %d\n", status); 
		}
		else { //если это дочерний процесс
			if (words_cnt == 1) { //если пользователь ввел только имя программы, без параметров
				if (execl(words_arr[0], words_arr[0], NULL) == -1) { //exec() замещает текущий процесс
					fprintf(stderr, "unable to exec(%s) \n", words_arr[0]);
					exit(EXIT_FAILURE);
				}	
			}
			//иначе надо снова сделать форк, чтобы заменить stdin для выполняемой программы
			//на созданный канал (pipe). Полученная инф-ия обрабатывается в process_params().
			pid_t pid1 = fork();
			switch (pid1) {
				case -1: {
					printf("error in fork()\n");
					exit(EXIT_FAILURE);
				}
				case 0: {
					int reserve_fd; //резервный file descriptor для stdin, который должен быть под номером 1
					if ((reserve_fd = dup( 1 )) == -1) { //dup ищет незанятый номер в таблице дескрипторов
						printf("error in dup(1)\n");
					}
					
					if (dup2(fildes[1], 1) == -1) { //поставить канал на место stdin
						fprintf(stderr, "error in dup2()\n");
					}				
					
					if (execl(words_arr[0], words_arr[0], NULL) == -1) { //exec() замещает текущий процесс
						fprintf(stderr, "unable to exec()\n");
						exit(EXIT_FAILURE);
					}				
					//код ниже вроде не должен выполняться, но на всякий случай оставлю
					if (dup2(reserve_fd, 1) == -1) { //вернуть stdin на свое место
						fprintf(stderr, "error in dup2(reserve_fd, 1)\n");
					}
				}
				default: {
					int status;
					waitpid(pid1, &status, 0);
					//printf("completed process status = %d\n", status);
					char buf[1024];
					ssize_t nbytes;
					nbytes = read(fildes[0], buf, 1024); //считать вывыденный текст программы, выбранной пользователем
					if (nbytes == -1) {
						fprintf(stderr, "error in read() from pipe\n");
						exit(EXIT_FAILURE);
					}
					//printf("\n");
					process_params(words_cnt, words_arr, 1, buf); //найти строку с искомым словом
					//printf("\n");
				}
			}
			exit(EXIT_SUCCESS);
		}
		
		free(input_string);
		for (int i = 0; i < words_cnt; i++) 
			free(words_arr[i]);
		free(words_arr);
	}
	
	exit(EXIT_FAILURE);
}
