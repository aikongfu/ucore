#include <stdio.h>

static int j;

int fun1(void) {
	static int i = 0;
	i++;
}

void fun2(void) {
	j = 0;
	j++;
}

int main() {
	int k;
	int m;
	for (k = 0; k < 10; k++) {
		m = fun1();
		fun2();
	}

	printf("m = [%d], j = [%d]\n", m, j);
	return 0;
}
