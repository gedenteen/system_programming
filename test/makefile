all: test0 test1 test2 test3 test4 test5

test0:
	gcc -Wall -g test0.c -o test0

test1:
	gcc -Wall -g test1.c -o test1

test2:
	gcc -g test2.c -o test2

test3:
	gcc -Wall -g test3.c -o test3
	
test4:
	gcc -Wall -g test4.c -o test4

test5:
	gcc -Wall -g test5.c -o test5 -ldl
	
run5:
	LD_LIBRARY_PATH=.:LD_LIBRARY_PATH ./test5
	
clean:
	rm -rf test?
