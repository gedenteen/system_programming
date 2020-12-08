struct bits {
    unsigned long long b0:1;
    unsigned long long b1:1;
    unsigned long long b2:1;
    unsigned long long b3:1;
    unsigned long long b4:1;
    unsigned long long b5:1;
    unsigned long long b6:1;
    unsigned long long b7:1;
};

union un {
    char c;
    int i;
};

int main()
{
    union un un1;
    un1.i = 1024 + 512 + 2;
    struct bits *bits1 = (struct bits*)&un1;

    int a[] = {1, 2, 3};
    return 0;
}
