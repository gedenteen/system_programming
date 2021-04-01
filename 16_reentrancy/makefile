all: create_object_files create_dynamic_lib linking

create_object_files: 
	gcc my_strerror.c -fPIC -c
	
create_dynamic_lib:
	gcc -shared my_strerror.o -o libmy_strerror.so
	
linking:
	gcc main.c -o main.exe -L. -lpthread -lmy_strerror
	
run:
	LD_LIBRARY_PATH=.:LD_LIBRARY_PATH ./main.exe
