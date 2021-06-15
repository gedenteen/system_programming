FLAGS = -Wall -g -c -o

all: client_TCP.exe client_UDP.exe server.exe

client_TCP.exe: client_TCP.o
	gcc -o $@ $^
client_TCP.o: client_TCP.c
	gcc $(FLAGS) $@ $<

client_UDP.exe: client_UDP.o
	gcc -o $@ $^
client_UDP.o: client_UDP.c
	gcc $(FLAGS) $@ $<

server.exe: server.o -lpthread -lrt
	gcc -o $@ $^
server.o: server.c
	gcc $(FLAGS) $@ $<

.PHONY: all clean run

clean:
	rm -rf *.o *.exe
