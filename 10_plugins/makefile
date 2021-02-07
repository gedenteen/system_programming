#OBJECT_FILES = obj/sum.o obj/sub.o obj/mul.o obj/div.o

all: create_object_files create_dynamic_lib linking

create_object_files: 
	gcc src/sum.c src/sub.c src/mul.c src/div.c src/bitwise_and.c -fPIC -c
	
create_dynamic_lib: libmysum libmysub libmymul libmydiv libmybitwise_and

libmysum:
	gcc -shared sum.o -o libmysum.so
	
libmysub:
	gcc -shared sub.o -o libmysub.so
		
libmymul:
	gcc -shared mul.o -o libmymul.so
	
libmydiv:
	gcc -shared div.o -o libmydiv.so
	
libmybitwise_and:
	gcc -shared bitwise_and.o -o libmybitwise_and.so
	
linking:
	gcc src/main.c -o main -ldl #ldl для dlfcn.h
	
run:
	LD_LIBRARY_PATH=.:LD_LIBRARY_PATH ./main libmymul.so libmybitwise_and.so
	
