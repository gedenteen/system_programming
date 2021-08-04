#include <stdio.h>

#define DEBUG_PRINT(val) \
	do { \
		if (DEBUG) printf("val == %d\n", val); \
	} while (0)

int main(void) {
	int val = 0;
	for (int i = 0; i < 3; i++) {
		val++;
		DEBUG_PRINT(val);
	}
	printf("val == %d\n", val);
	return 0;
}
