#include <stdio.h>

int main() {
	int command = 0, num1, num2;
	while (1) {
		printf("calculator:\n"); 
		printf("1. sum (+)\n");
		printf("2. subtraction (-) \n");
		printf("3. multiplication (*) \n");
		printf("4. division (/) \n");
		printf("enter number of command and 2 numbers: ");
		scanf("%d %d %d", &command, &num1, &num2);
		switch (command) {
		case 1:
			printf("\nresult = %d \n\n", sum(num1, num2));
			break;
		case 2:
			printf("\nresult = %d \n\n", sub(num1, num2));
			break;
		case 3:
			printf("\nresult = %d \n\n", mul(num1, num2));
			break;
		case 4:
			printf("\nresult = %d \n\n", div(num1, num2));
			break;
		default:
			break;
		}
		char ans;
		printf("continue? (y or n) \n");
		scanf("\n%c", &ans);
		if (ans == 'n')
			break;
	}
	return 0;
}
