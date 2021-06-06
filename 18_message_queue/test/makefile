all: SystemV_1.exe SystemV_2.exe POSIX_1.exe POSIX_2.exe

SystemV_1.exe: SystemV_1.o
	gcc -o $@ $^
SystemV_1.o: SystemV_1.c
	gcc -g -c -o $@ $<
	
SystemV_2.exe: SystemV_2.o
	gcc -o $@ $^
SystemV_2.o: SystemV_2.c
	gcc -g -c -o $@ $<
	
POSIX_1.exe: POSIX_1.o
	gcc -o $@ $^ -lrt
POSIX_1.o: POSIX_1.c
	gcc -g -c -o $@ $<

POSIX_2.exe: POSIX_2.o
	gcc -o $@ $^ -lrt
POSIX_2.o: POSIX_2.c
	gcc -g -c -o $@ $<

.PHONY: all clean run

clean:
	rm -rf *.o *.exe
