#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void several_conditions() {
    int val = rand() % 8;
    if (0 == val)
        printf("0 ");
    if (1 == val)
        printf("1 ");
    if (2 == val)
        printf("2 ");
    if (3 == val)
        printf("3 ");
    if (4 == val)
        printf("4 ");
    if (5 == val)
        printf("5 ");
    if (6 == val)
        printf("6 ");
    if (7 == val)
        printf("7 ");

}

void one_condition() {
    int val = rand() % 8;
    if (0 == val)
        printf("0 ");
    else if (1 == val)
        printf("1 ");
    else if (2 == val)
        printf("2 ");
    else if (3 == val)
        printf("3 ");
    else if (4 == val)
        printf("4 ");
    else if (5 == val)
        printf("5 ");
    else if (6 == val)
        printf("6 ");
    else
        printf("7 ");

}

int main() {
    clock_t start, stop, time_several, time_one;

    start = clock();
    for (long i = 0; i < 1e7; i++) {
        several_conditions();
    }
    stop = clock();
    time_several = stop - start;

    start = clock();
    for (long i = 0; i < 1e7; i++) {
        one_condition();
    }
    stop = clock();
    time_one = stop - start;

    printf("time for several_conditions() = %ld \n", time_several);
    printf("time for one_condition() = %ld \n", time_one);
    return 0;
}
