#include <stdio.h>
#include <sys/types.h> //для fork() и getpid()
#include <unistd.h> //для fork() и getpid()

int main() {
	int PID;
	PID = fork(); //возвращает PID дочернего процесса для родителя, 
	//и 0 для дочернего процесса
	if (PID) {
		PID = getpid();
		printf("I am a parent, my PID = %d\n", PID);
	}
	else {
		PID = getpid();
		printf("I am a child, my PID = %d\n", PID);
	}

	return 0;
}
