#include "my_includes.h"

int sum(int a, int b) {
	return a + b;
}

char* get_func_name() {
	char *func_name = (char*) malloc(4 * sizeof(char));
	strcpy(func_name, "sum\0");
	return func_name;
}
