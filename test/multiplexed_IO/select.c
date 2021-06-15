//код из книги Robert Love "Linux system programming"

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define TIMEOUT 5 //ожидание в секундах для select()
#define BUF_LEN 1024 //длина буфера считывания в байтах

int main(void)
{
	fd_set readfds; //набор файловых дескрипторов на чтение
	int ret;

	/// Ожидать ввода на stdin:
	FD_ZERO(&readfds); //макрос на удаление всех дескрипторов
	FD_SET(STDIN_FILENO, &readfds); //макрос на добавление дескриптора

	/// время ожидания:
	struct timeval tv; 
	tv.tv_sec = TIMEOUT;
	tv.tv_usec = 0;

	/// Вызов select() блокируеутся до тех пор, пока один из указанных ФД
	/// не станет доступен для ввода или вывода, или же пока не закончится
	/// указанное время:
	ret = select(STDIN_FILENO + 1, //максимальное значение среди всех ФД + 1
	             &readfds, //набор ФД на чтение
	             NULL, //набор ФД на запись
	             NULL, //набор ФД на ??
	             &tv); //время ожидание, можно NULL
	if (ret == -1) {
		perror("error in select()");
		return 1;
	} else if (!ret) { //ничего не готово и время закончилось
		printf("%d seconds elapsed \n", TIMEOUT);
		return 0;
	}

	/// еще раз проверка на то, что stdin готов к чтению:
	if (FD_ISSET(STDIN_FILENO, &readfds)) {
		char buf[BUF_LEN + 1];
		int len = read(STDIN_FILENO, buf, BUF_LEN); //без блокировки
		if (len == -1) {
			perror("error in read()");
			return 1;
		}

		if (len) {
			buf[len] = '\0';
			printf("read %s\n", buf);
		}
		return 0;
	}
	
	fprintf(stderr, "ya suda ne dolzhen byl popast'\n");
	return 1;
}