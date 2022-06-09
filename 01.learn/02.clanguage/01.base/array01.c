#include <stdio.h>

int main() {
    int array[] = {2, 4, 6, 8, 10};

    int *p = &array[3];

    printf("%d\n", (p-array));
}
