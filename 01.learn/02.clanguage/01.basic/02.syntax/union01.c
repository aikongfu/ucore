#include <stdio.h>

union data {
    int n;
    char ch;
    short m;

};

int main() {
    union data d;

    printf("%d, %d\n", sizeof(d), sizeof(union data));
    d.n = 0x40;
    printf("%X, %c, %hX\n", d.n, d.ch, d.m);

    d.ch = '9';
    printf("%X, %c, %hX\n", d.n, d.ch, d.m);

    d.m = 0x2059;
    printf("%X, %c, %hX\n", d.n, d.ch, d.m);

    d.n = 0x3E25AD54;
    printf("%X, %c, %hX\n", d.n, d.ch, d.m);

    return 0;
}


