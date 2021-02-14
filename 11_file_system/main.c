#include <fcntl.h> //для open()
#include <unistd.h> //для read(), sync()
#include <stdio.h>
#include <string.h>
#include <malloc.h>

int main() {
	int fd = open("./text.txt", O_CREAT | O_WRONLY); //fd = file descriptor
	if (fd == -1) {
		printf("error: can't open file text.txt\n");
		return 1;
	}
	char* string; //строка для записи в файл
	string = (char*) malloc(strlen("hello, world!") * sizeof(char) + 1); //+ 1 для '\0'
	strcpy(string, "hello, world!");	
	write(fd, string, strlen("hello, world!")); //написать "hello, world" в файл
	sync(); //записать все write buffers на диск
	close(fd);
	
	struct stat buf; //структура для stat(), который получает инф-ию о файле
	if (stat("./text.txt", &buf) == -1) {
		printf("error in stat()\n");
		return 1;
	}
	long int bytes = buf.st_size;
	printf("file have %ld bytes\n", bytes);
	
	fd = open("./text.txt", O_RDONLY);
	char ch[1];
	for (long int i = 1; i <= bytes; i++) {
		lseek(fd, -1 * i, SEEK_END); //изменить указатель, откуда считывать файл
		read(fd, ch, 1); //считать один байт
		printf("%c", ch[0]); //и вывести его
	}
	printf("\n");
	close(fd);
	return 0;
}
