#include <stdio.h>
#include <dlfcn.h>

int main() {
	printf("hi\n");
	void *handle; //дескриптор
	handle = dlopen("lib_mymath.so", RTLD_LAZY);
	if (!handle) {
		fputs(dlerror(), stderr);
		return 1;
	}
	
	int (*func_ptr)(int, int);
	func_ptr = dlsym(handle, "mul");
	if (!func_ptr) {
		fputs(dlerror(), stderr);
		return 1;
	}
	printf("mul(3, 5) = %d \n", func_ptr(3, 5));
	
	return 0;
}
