//код из книги Robert Love "Linux system programming"

#include <stdio.h>
#include <sys/poll.h>
#include <unistd.h>

#define TIMEOUT 5 //ожидание в секундах для poll()
#define BUF_LEN 1024 //длина буфера считывания в байтах

int main(void)
{
	struct pollfd fds[2];
	int ret;

	/// Ожидать ввод на stdin
	fds[0].fd = STDIN_FILENO;
	fds[0].events = POLLIN;

	/// Наблюдать stdout - возможна ли запись
	/// (практически всегда это так)
	fds[1].fd = STDOUT_FILENO;
	fds[1].events = POLLOUT;

	/// Все установлено, можно фиксировать
	ret = poll(fds, 2, TIMEOUT * 1000);
	if (ret == -1) {
		perror("error in poll()");
		return 1;
	}

	if (!ret) {
		printf("%d seconds elapsed.\n", TIMEOUT);
		return 0;
	}

	if (fds[0].revents & POLLIN)
		printf("stdin is readable\n");

	if (fds[1].revents & POLLOUT)
		printf("stdout is writable\n");

	return 0;
}