#include <stdio.h>
#include <stdint.h>

void showBytes(int num) {
    printf("num = %08X \n", num);
    //uint8_t *ptr = (uint8_t*)&num;
    unsigned char *ptr = (unsigned char*)&num;
    for(int i = 1; i <= 4; i++) {
        printf("byte %d = %X \n", i, *ptr);
        ptr++;
    }
}

void changeByte(int num, int indByte, int value) {
    printf("old num = %08X, indByte = %d, value = %X \n",
           num, indByte, value);
    unsigned char *ptr = (unsigned char*)&num;
    ptr += indByte - 1; ///если считать байты с 1
    *ptr += value;
    printf("new num = %08X \n", num);
}

int main() {
    printf("\n showBytes():\n");
    showBytes((int)0x11BB00DC);
    showBytes((int)0x0110A00A);
    printf("\n changeByte():\n");
    changeByte((int)0x11BB00DC, 4, 0x11);
    changeByte((int)0x11BB00DC, 3, -0xB0);

    return 0;
}
