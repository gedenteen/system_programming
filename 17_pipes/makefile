all: main.exe
	
main.exe: main.o
	gcc -o $@ $<
main.o: main.c
	gcc -Wall -g -c -o $@ $<
