#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <malloc.h>

#define STRING_LEN 256

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

int main(int argc, char *argv[]) {
	char *input_string = (char*) malloc (STRING_LEN * sizeof(char)); 
	printf("imput_string before fgets() = %s.\n", input_string); //отладка
	printf("enter the some words: ");		
	fgets(input_string, STRING_LEN, stdin); //считать строку с ' ' до '\n'
	int words_cnt = 0;
	char **words_arr = (char**) malloc(sizeof(char*)); //без маллока здесь ошибка при реаллоке в highlight_words()

	highlight_words(input_string, &words_cnt, &words_arr);
	for (int i = 0; i < words_cnt; i++) {
		printf("words_arr[%d] = %s\n", i, words_arr[i]);
	} 

	return 0;
 }
