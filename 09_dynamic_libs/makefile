all: create_object_files create_dynamic_lib linking

create_object_files: 
	gcc sum.c sub.c mul.c div.c -fPIC -c
	
create_dynamic_lib:
	gcc -shared sum.o sub.o mul.o div.o -o lib_mymath.so
	
linking:
	gcc main.c -o main.exe -L. -l_mymath
	
run:
	LD_LIBRARY_PATH=.:LD_LIBRARY_PATH ./main.exe
	
