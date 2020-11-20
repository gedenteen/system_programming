#include <stdio.h>

#define N 5

void task1(int array[][N]) {
///вывести квадратную матрицу
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            printf("%2d ", array[i][j]);
        printf("\n");
    }
}

void task2() {
///вывести заданный массив размером N в обратном порядке
    printf("enter %d numbers: \n", N);
    int i, array[N];
    for (i = 0; i < N; i++)
        scanf("%d", &array[i]);

    printf("array in reverse order: \n");
    for (i = N - 1; i >= 0; i--)
        printf("%d ", array[i]);
    printf("\n");
}

void task3(int array[][N]) {
///заполнить верхний треугольник матрицы 0, а нижний 1
    int j_max, i, j;
    for (i = 0, j_max = N - 1; i < N; i++, j_max--) {
        for (j = 0; j < j_max; j++)
            array[i][j] = 0;
        for (j = j_max; j < N; j++)
            array[i][j] = 1;
        //printf("j_max = %d \n", j_max);
    }
}

void task4(int array[][N]) {
///заполнить матрицу числами от 1 до N^2 улиткой
    int i, j;
    int num = 1; ///число, которое будет записываться
    int sq = 0; ///какой квадрат по счету
    for (num = 1, sq = 0; num <= N * N; sq++) {
        for (i = sq, j = sq; j < N - sq; j++, num++)
            array[i][j] = num;
        for (j = N - 1 - sq, i++; i < N - sq; i++, num++)
            array[i][j] = num;
        for (i = N - 1 - sq, j--; j >= sq; j--, num++)
            array[i][j] = num;
        for (j = sq, i--; i > sq; i--, num++)
            array[i][j] = num;
    }
}

int main() {
    int array[N][N] = {0};

    printf("\ntask1: \n");
    task1(array);
    printf("\ntask2: \n");
    task2();
    printf("\ntask3: \n");
    task3(array); task1(array);
    printf("\ntask4: \n");
    task4(array); task1(array);

    return 0;
}
