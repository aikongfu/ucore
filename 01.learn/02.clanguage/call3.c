#include <stdio.h>


int f3(int t) {
	return t + 5;
}

void f2() {
	int x = 8;
	int y = f3(x);
	int z = y;
}

void f1() {
	int x = 7;
	f2();
}



int main(int argc, char *argv) {
  f1();
}
