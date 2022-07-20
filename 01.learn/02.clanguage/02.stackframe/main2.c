int foo();
int bar();

int main(void)
{
	foo(2, 3, 4, 5, 6, 7, 8, 9, 10);
	return 0;
}

int foo(int a, int b)
{
	return bar(a, b, 11);
}

int bar(int c, int d)
{
	int e = c + d;
	return e;
}
