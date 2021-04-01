all: test0 test1 test2 test3 test4 test5 test6 test7 test8 \
 cursed0 cursed1 cursed2 cursed3 cursed4 cursed5

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

test6:
	gcc -Wall -g test6.c -o test6
	
test7:
	gcc -Wall -g test7.c -o test7
	
test8:
	gcc -Wall -g test8.c -o test8 -lpthread

cursed0:
	gcc cursed0.c -o cursed0.exe -lncurses
	
cursed1:
	gcc cursed1.c -o cursed1.exe -lncurses
		
cursed2:
	gcc cursed2.c -o cursed2.exe -lncurses
		
cursed3:
	gcc cursed3.c -o cursed3.exe -lncurses
		
cursed4:
	gcc cursed4.c -o cursed4.exe -lncurses
	
cursed5:
	gcc cursed5.c -o cursed5.exe -lncurses
	
clean:
	rm -rf test?
