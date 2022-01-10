#include <stdio.h>

struct {
  int x : 4;
  int y : 4;
  int z : 4;
} position;

struct position *foo(int a, int b, int c) {

  int m = a * 100 + b * 10 + c;

  struct position *pos = {100 * a, 10 * b, c};
  return pos;
}

void bar() {

  int l = 9;
  int m = 8;
  int n = 7;
  struct position *xx;
  xx = foo(l, m, n);
}

int main() {
  bar();

  return 0;
}
