STANDARD_FLAGS = -Wall -g -c -o

all: cursed0.bin cursed1.bin cursed2.bin \
     cursed3.bin cursed4.bin cursed5.bin
     
#создание исполняемого файла:
%.bin: %.o
	gcc -o $@ $^  -lncurses
	
#создание объектного файла:
%.o: %.c
	gcc $(STANDARD_FLAGS) $@ $<

clean:
	rm -rf *.o *.bin
