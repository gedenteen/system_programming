#include <stdio.h>

int main() {
    float x = 1.0;
    while(1) {
        printf("%.15f \n", x);
        x += 1.0;
    }
    return 0;
}
