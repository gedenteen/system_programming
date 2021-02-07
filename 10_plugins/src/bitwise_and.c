#include "my_includes.h"

int bitwise_and(int a, int b) {
	return a & b;
}

char* get_func_name() {
	char *func_name = (char*) malloc(12 * sizeof(char));
	strcpy(func_name, "bitwise_and\0");
	return func_name;
}
