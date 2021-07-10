all: server.bin client.bin

client.bin: client.o
	gcc -o $@ $^
client.o: client.c
	gcc -g -c -o $@ $<

server.bin: server.o
	gcc -o $@ $^
server.o: server.c
	gcc -g -c -o $@ $<

.PHONY: all clean run

clean:
	rm -rf *.o *.bin
