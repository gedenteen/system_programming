#include <stdio.h>
#include <sys/types.h>
#include <unistd.h> 
#include <wait.h>

void print_PID_and_PPID() {
	pid_t pid = getpid();
	pid_t ppid = getppid();
	printf("I am a process with PID = %d and PPID = %d\n", pid, ppid);
}

int main() {
	pid_t pid_2, pid_3;
	pid_t pid_4, pid_5;
	pid_2 = fork(); 
	pid_3 = fork(); 
	
	if (pid_2 && pid_3) { //ветка для 1-го процесса
		waitpid(pid_2, NULL, 0); //подождать процесс потомка, не сохранять его статус, без доп. опций
		waitpid(pid_3, NULL, 0);
		print_PID_and_PPID();
	}
	
	if (pid_2 && pid_3 == 0) { //ветка для потомка (2-ой процесс)
		pid_4 = fork(); 
		pid_5 = fork();
		
		if (pid_4 && pid_5) { //ветка для родителя (2-ой процесс)
			waitpid(pid_4, NULL, 0);
			waitpid(pid_5, NULL, 0);
			print_PID_and_PPID();
		}
		if (pid_4 && pid_5 == 0) { //ветка для потомка (4-ой процесс)
			print_PID_and_PPID();
		} 
		if (pid_4 == 0 && pid_5) { //ветка для потомка (5-ой процесс)
			print_PID_and_PPID();
		}
	}
	
	if (pid_2 == 0 && pid_3) { //ветка для 2-го потомка (3-ий процесс)
		pid_t pid_6 = fork();
		
		if (pid_6 == 0) { //ветка для потомка (6-ой процесс)
			print_PID_and_PPID();
		}
		else { //ветка для родителя (3-ий процесс)
			waitpid(pid_6, NULL, 0);
			print_PID_and_PPID();
		}
	}

	return 0;
}
