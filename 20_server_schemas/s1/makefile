all: client.exe server.exe

client.exe: client.o
	gcc -o $@ $^
client.o: client.c
	gcc -g -c -o $@ $<

server.exe: server.o -lpthread
	gcc -o $@ $^
server.o: server.c
	gcc -g -c -o $@ $<

.PHONY: all clean run

clean:
	rm -rf *.o *.exe
