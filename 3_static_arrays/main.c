#include <stdio.h>
#include <stdlib.h>

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

int task5(int rez_arr[][N]) {
///построение магичечкого квадрата, метод террас
    if (N % 2 == 0) {
        printf("for even N is not implemented \n");
        return 1;
    }
    ///Выделение памяти:
    int M = N + N - 1; ///размерность для вспомогательной матрицы
    int i_beg, j_beg, i, j;
    int **tmp_arr = (int**) malloc(M * sizeof(int*)); ///вспомогательная матрица
    for(i = 0; i < M; i++) {
        tmp_arr[i] = (int*) malloc(M * sizeof(int));
    }
    ///выбор диагонали:
    int num = 1;
    for (i_beg = M / 2, j_beg = 0; i_beg < M; i_beg++, j_beg++) {
        ///проход по диагонали:
        int cnt = 0;
        for (i = i_beg, j = j_beg; cnt < N; i--, j++, cnt++, num++) {
            tmp_arr[i][j] = num;
        }
    }
    ///проверка:
    /*for (i = 0; i < M; i++) {
        for (j = 0; j < M; j++)
            printf("%2d ", tmp_arr[i][j]);
        printf("\n");
    }*/
    ///перемещение террас:
    for (j = N / 2 + 1; j < N / 2 + 1 + N ; j++) {
        for (i = 0; i <= N / 2 ; i++) { ///сверху
            if (tmp_arr[i][j] != 0)
                tmp_arr[i + N][j] = tmp_arr[i][j];
        }
        for (i = N / 2 + N; i < M ; i++) { ///снизу
            if (tmp_arr[i][j] != 0)
                tmp_arr[i - N][j] = tmp_arr[i][j];
        }
    }
    for (i = N / 2 + 1; i < N / 2 + 1 + N ; i++) {
        for (j = 0; j <= N / 2 ; j++) { ///слева
            if (tmp_arr[i][j] != 0)
                tmp_arr[i][j + N] = tmp_arr[i][j];
        }
        for (j = N / 2 + N; j < M ; j++) { ///справа
            if (tmp_arr[i][j] != 0)
                tmp_arr[i][j - N] = tmp_arr[i][j];
        }
    }
    ///запись результата:
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            rez_arr[i][j] = tmp_arr[i + N / 2][j + N / 2];
        }
    }
    ///освобождение памяти:
    for(i = 0; i < M; i++)
        free(tmp_arr[i]);
    free(tmp_arr);
    return 0;
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
    printf("\ntask5: \n");
    if (task5(array) == 0)
        task1(array);

    return 0;
}
