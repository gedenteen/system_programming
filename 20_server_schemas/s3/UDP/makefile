FLAGS = -Wall -g -c -o

all: client.exe server.exe

client.exe: client.o
	gcc -o $@ $^
client.o: client.c
	gcc $(FLAGS) $@ $<

server.exe: server.o -lpthread -lrt
	gcc -o $@ $^
server.o: server.c
	gcc $(FLAGS) $@ $<

.PHONY: all clean run

clean:
	rm -rf *.o *.exe
