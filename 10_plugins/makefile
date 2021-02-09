OBJECT_FILES = obj/sum.o obj/sub.o obj/mul.o obj/div.o obj/bitwise_and.o
SOURCE_FILES = src/sum.c src/sub.c src/mul.c src/div.c src/bitwise_and.c

all: create_object_files create_dynamic_lib linking

create_object_files: $(OBJECT_FILES)
	
obj/sum.o:
	gcc -o obj/sum.o -fPIC -c src/sum.c
	
obj/sub.o:
	gcc -o obj/sub.o -fPIC -c src/sub.c
	
obj/mul.o:
	gcc -o obj/mul.o -fPIC -c src/mul.c
	
obj/div.o:
	gcc -o obj/div.o -fPIC -c src/div.c
	
obj/bitwise_and.o:
	gcc -o obj/bitwise_and.o -fPIC -c src/bitwise_and.c
	
create_dynamic_lib: libmysum libmysub libmymul libmydiv libmybitwise_and

libmysum:
	gcc -shared obj/sum.o -o libmysum.so
	
libmysub:
	gcc -shared obj/sub.o -o libmysub.so
		
libmymul:
	gcc -shared obj/mul.o -o libmymul.so
	
libmydiv:
	gcc -shared obj/div.o -o libmydiv.so
	
libmybitwise_and:
	gcc -shared obj/bitwise_and.o -o libmybitwise_and.so
	
linking:
	gcc src/main.c -o main -ldl #ldl для dlfcn.h
	
run:
	LD_LIBRARY_PATH=.:LD_LIBRARY_PATH ./main libmymul.so libmybitwise_and.so
	
