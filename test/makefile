STANDARD_FLAGS = -Wall -g -c -o

all: test0.bin test1.bin test2.bin test3.bin test4.bin \
test5.bin test6.bin test7.bin test8.bin test9.bin test10.bin \
test11.bin test13.bin

test0.bin: test0.o
	gcc -o $@ $<
test0.o: test0.c
	gcc $(STANDARD_FLAGS) $@ $<

test1.bin: test1.o
	gcc -o $@ $<
test1.o: test1.c
	gcc -O3 $(STANDARD_FLAGS) $@ $<

test2.bin: test2.o
	gcc -o $@ $<
test2.o: test2.c
	gcc -g -c -o $@ $<

test3.bin: test3.o
	gcc -o $@ $<
test3.o: test3.c
	gcc $(STANDARD_FLAGS) $@ $<
	
test4.bin: test4.o
	gcc -o $@ $<
test4.o: test4.c
	gcc $(STANDARD_FLAGS) $@ $<

test5.bin: test5.o
	gcc -o $@ $< -ldl
test5.o: test5.c
	gcc $(STANDARD_FLAGS) $@ $< 
	
run5:
	LD_LIBRARY_PATH=.:LD_LIBRARY_PATH ./test5.bin

test6.bin: test6.o
	gcc -o $@ $<
test6.o: test6.c
	gcc -Wall -g -c -o $@ $<
	
test7.bin: test7.o
	gcc -o $@ $<
test7.o: test7.c
	gcc $(STANDARD_FLAGS) $@ $<
	
test8.bin: test8.o
	gcc -o $@ $< -lpthread
test8.o: test8.c
	gcc $(STANDARD_FLAGS) $@ $<
	
test9.bin: test9.o
	gcc -o $@ $<
test9.o: test9.c
	gcc $(STANDARD_FLAGS) $@ $<
	
test10.bin: test10.o
	gcc -o $@ $<
test10.o: test10.c
	gcc $(STANDARD_FLAGS) $@ $<
	
test11.bin: test11.o
	gcc -o $@ $<
test11.o: test11.c
	gcc $(STANDARD_FLAGS) $@ $<

test13.bin: test13.o
	gcc -o $@ $<
test13.o: test13.c
	gcc $(STANDARD_FLAGS) $@ $< -DDEBUG=0

clean:
	rm -rf *.o *.bin
