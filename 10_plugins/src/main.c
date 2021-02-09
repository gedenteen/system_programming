#include "my_includes.h"

int ProcessParameters(int argc, char *argv[]) {
    int i;
    for (i = 1; i < argc; i++) {
        ///если размер матрицы:
        if (strcmp("-s", argv[i]) == 0) {
            printf("success\n");
            /*
            i++;
            matrixSize = atoi(argv[i]); ///приведение строки в long long int
            if (matrixSize == 0) {
                printf("Error in arguments of main(): incorrect value for --launch-count \n");
                return 1;
            }
            */
        }
    }

    return 0;
}

//int get_func_from_plugin

int main(int argc, char *argv[]) {
	int i;
	//массив указателей на функции из плагинов:
	int (**func_ptr)(int, int); 
	//выделить память под этот массив:
	func_ptr = (int (**)(int,  int)) malloc(argc * sizeof(int*));
	//массив имен функций из плагинов:
	char **func_name = (char**) malloc(argc * sizeof(char*)); 
	
	//подгрузить функции из плагинов, имена плагинов задаются как параметры main():
	for (i = 1; i < argc; i++) {
		void *handle; //дескриптор 
		handle = dlopen(argv[i], RTLD_LAZY); //открыть плагин
		if (!handle) {
			fputs(dlerror(), stderr);
			return 1;
		}
		
		char* (*temp_func)(); 
		temp_func = dlsym(handle, "get_func_name"); //получить имя функции из плагина
		// функция get_func_name() определена во всех библиотеках
		if (!temp_func) {
			fputs(dlerror(), stderr);
			return 1;
		}
		
		func_name[i] = temp_func(); //получить имя библ-ой фу-ии
		if (!func_name[i]) {
			fputs("error: can't get func_name from lib\n", stderr);
			return 1;
		}
		
		func_ptr[i] = dlsym(handle, func_name[i]); //создать указатель на эту фу-ию
		if (!func_ptr[i]) {
			fputs(dlerror(), stderr);
			return 1;
		}
	}
	
	//все функции из плагинов получены, теперь можно использовать калькулятор:
	int command = 0, num1, num2;
	while (1) {
		//перечисление всех доступных функций:
		printf("functions (all of them have parameters int, int):\n"); 
		for (i = 1; i < argc; i++) {
			printf("%d. %s \n", i, func_name[i]);
		}
		//использование одной из функций:
		printf("enter number of command and 2 numbers: ");
		scanf("%d %d %d", &command, &num1, &num2);
		if (command >= 1 && command < argc)
			printf("\nresult = %d \n\n", func_ptr[command](num1, num2));
		else
			printf("wrong number of command\n");
		//узнать, надо ли продолжить выполнение программы:
		char ans;
		printf("continue? (y or n) \n");
		scanf("\n%c", &ans);
		if (ans == 'n')
			break;
	}
	
	return 0;
}
