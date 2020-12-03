#include <stdio.h>
#include <time.h>

struct test1 {
    char a;
    int b;
};

struct test2 {
    int a;
    char b;
};

struct test3 {
    char a;
    int b;
} __attribute__((packed));

struct test4 {
    char a;
    int b;
    char c;
    int d;
    char e;
    int f;
};

struct test5 {
    int b;
    int d;
    int f;
    char a;
    char c;
    char e;
};

int main() {
    int i;
    printf("address int i = %p \n", &i);

    printf("sizeof(struct test1) = %ld \n", sizeof(struct test1));
    printf("sizeof(struct test2) = %ld \n", sizeof(struct test2));
    printf("sizeof(struct test3) = %ld \n", sizeof(struct test3));
    struct test1 t1;
    printf("address t1.a = %p; t1.b = %p \n", &t1.a, &t1.b);
    struct test2 t2;
    printf("address t2.a = %p; t2.b = %p \n", &t2.a, &t2.b);
    struct test3 t3;
    printf("address t3.a = %p; t3.b = %p \n", &t3.a, &t3.b);

    printf("sizeof(struct test4) = %ld \n", sizeof(struct test4));
    printf("sizeof(struct test5) = %ld \n", sizeof(struct test5));

    ///сравнение времени:
    clock_t start_t, stop_t;
    //t1.b = 0;
    start_t = clock();
    for (long long j = 0; j < 1e9; j++) {
        t1.b += 2;
    }
    stop_t = clock();
    printf("t1.b = %d \n", t1.b);
    printf("ticks for struct test1 = %ld \n", stop_t - start_t);

    //t3.b = 0;
    start_t = clock();
    for (long long j = 0; j < 1e9; j++) {
        t3.b += 2;
    }
    stop_t = clock();
    printf("t3.b = %d \n", t3.b);
    printf("ticks for struct test3 = %ld \n", stop_t - start_t);

    return 0;
}
