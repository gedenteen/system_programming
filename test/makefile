STANDARD_FLAGS = -Wall -g -c -o

all: test0.exe test1.exe test2.exe test3.exe test4.exe \
test5.exe test6.exe test7.exe test8.exe test9.exe test10.exe\
cursed0.exe cursed1.exe cursed2.exe \
cursed3.exe cursed4.exe cursed5.exe

test0.exe: test0.o
	gcc -o $@ $<
test0.o: test0.c
	gcc $(STANDARD_FLAGS) $@ $<

test1.exe: test1.o
	gcc -o $@ $<
test1.o: test1.c
	gcc $(STANDARD_FLAGS) $@ $<

test2.exe: test2.o
	gcc -o $@ $<
test2.o: test2.c
	gcc -g -c -o $@ $<

test3.exe: test3.o
	gcc -o $@ $<
test3.o: test3.c
	gcc $(STANDARD_FLAGS) $@ $<
	
test4.exe: test4.o
	gcc -o $@ $<
test4.o: test4.c
	gcc $(STANDARD_FLAGS) $@ $<

test5.exe: test5.o
	gcc -o $@ $< -ldl
test5.o: test5.c
	gcc $(STANDARD_FLAGS) $@ $< 
	
run5:
	LD_LIBRARY_PATH=.:LD_LIBRARY_PATH ./test5.exe

test6.exe: test6.o
	gcc -o $@ $<
test6.o: test6.c
	gcc -Wall -g -c -o $@ $<
	
test7.exe: test7.o
	gcc -o $@ $<
test7.o: test7.c
	gcc $(STANDARD_FLAGS) $@ $<
	
test8.exe: test8.o
	gcc -o $@ $< -lpthread
test8.o: test8.c
	gcc $(STANDARD_FLAGS) $@ $<
	
test9.exe: test9.o
	gcc -o $@ $<
test9.o: test9.c
	gcc $(STANDARD_FLAGS) $@ $<
	
test10.exe: test10.o
	gcc -o $@ $<
test10.o: test10.c
	gcc $(STANDARD_FLAGS) $@ $<

#TODO: доделать ниже
cursed0.exe:
	gcc cursed0.c -o cursed0.exe -lncurses
	
cursed1.exe:
	gcc cursed1.c -o cursed1.exe -lncurses
		
cursed2.exe:
	gcc cursed2.c -o cursed2.exe -lncurses
		
cursed3.exe:
	gcc cursed3.c -o cursed3.exe -lncurses
		
cursed4.exe:
	gcc cursed4.c -o cursed4.exe -lncurses
	
cursed5.exe:
	gcc cursed5.c -o cursed5.exe -lncurses
	
clean:
	rm -rf test?
