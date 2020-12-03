#include <stdio.h>

struct test1 {
    char a;
    int b;
};

struct test2 {
    char a;
    int b;
} __attribute__((packed));

int main() {
    char str[10] = {'A', 0, 0, 0, 0, 'B', 0, 0, 0, 0};

    printf("struct test1: \n");
    struct test1 *ptr = (struct test1*) str;
    printf("ptr->a = %c; ptr->b = %d \n", ptr->a, ptr->b);
    ptr++;
    printf("ptr->a = %c; ptr->b = %d \n", ptr->a, ptr->b);

    printf("struct test2: \n");
    struct test2 *ptr2 = (struct test2*) &str;
    printf("ptr2->a = %c; ptr2->b = %d \n", ptr2->a, ptr2->b);
    ptr2++;
    printf("ptr2->a = %c; ptr2->b = %d \n", ptr2->a, ptr2->b);

    return 0;
}
