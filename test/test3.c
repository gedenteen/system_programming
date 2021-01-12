#include <stdio.h>

int func(int param1) {
    int arr[3];
    printf("enter arr[3]: ");
    for (int i = 0; i < 4; i++)
        scanf("%d", &arr[i]);
    for (int i = 0; i < param1; i++)
        printf("arr[%d] = %d \n", i, arr[i]);
    return 0;
}

int main(void) {
    int b = 2;
    func(b);
    return 0;
}
