#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void task1(int num) {
///вывод 4-ех байтов числа
    printf("num = %08X \n", num);
    int temp = 0xFF;
    for (int i = 1; i <= 4; i++) {
        printf("byte %d is %X \n", i, num & temp);
        num = num >> 8;
    }
}

void task2(int num, int byte) {
///изменение 3-го байта в числе
    printf("old num = %08X, byte = %X \n", num, byte);
    int temp = 0xFF00FFFF;
    num = num & temp; ///зануление 3-го байта
    byte = byte << 16;
    num = num | byte;
    printf("new num = %08X \n", num);
}

int main() {
    printf("task1: \n");
    task1(0x00DDAABB);
    task1(16 + 1024 + (0xC << 24)); //1024 = 2^8 * 4
    printf("\ntask2: \n");
    task2(0x00DDAABB, 0x1F);
    task2(0x00DDAABB, 0);
    task2(0x44332211, 0xB0);
    return 0;
}
