all: create_object_files create_archive linking

create_object_files: 
	gcc sum.c sub.c mul.c div.c -c
	
create_archive:
	ar rc lib_mymath.a sum.o sub.o mul.o div.o
	
linking:
	gcc main.c -o main.exe -L. -l_mymath
	
