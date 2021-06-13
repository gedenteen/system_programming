STANDARD_FLAGS = -Wall -g -c -o

all: select.exe poll.exe

select.exe: select.o
	gcc -o $@ $<
select.o: select.c
	gcc $(STANDARD_FLAGS) $@ $<

poll.exe: poll.o
	gcc -o $@ $<
poll.o: poll.c
	gcc $(STANDARD_FLAGS) $@ $<

.PHONY: all clean run

clean:
	rm -rf *.o *.exe
