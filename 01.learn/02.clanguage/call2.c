#include <stdio.h>

typedef int int32_t;
typedef unsigned int uint32_t;

typedef uint32_t uintptr_t;

struct {
  int x : 4;
  int y : 4;
  int z : 4;
} position;

static inline uint32_t read_ebp(void) {
  uint32_t ebp;
  asm volatile("movl %%ebp, %0" : "=r"(ebp));
  return ebp;
}

static inline uint32_t read_eip(void) {
  uint32_t eip;
  asm volatile("movl 4(%%ebp), %0" : "=r"(eip));
  return eip;
}

struct position *foo(int a, int b, int c) {
  uint32_t eip = read_eip();
  uint32_t ebp = read_ebp();

  long long m = a * 100 + b * 10 + c;

  uint32_t eip1 = read_eip();
  uint32_t ebp1 = read_ebp();
  return m;
}

void bar() {
  int l = 2;
  int m = 3;
  int n = 5;

  struct position *x;
  x = foo(9, 8, 7);

  int z = l + m + n;
}

int f3(int t) {
	return t + 5;
}

void f2() {
	int x = 1;
	int y = f3(x);
	int z = y;
}

void f1() {
	int x = 1;
	f2();
}



int main(int argc, char *argv) {
  uint32_t eip = read_eip();
  uint32_t ebp = read_ebp();
  bar();
  f1();
  uint32_t eip1 = read_eip();
  uint32_t ebp1 = read_ebp();
}
