#include <stdio.h>

int main(int argc, const char *argv[])
{
	int a = 10, b = 20;

	a = a ^ b;
	b = a ^ b;
	a = a ^ b;

	printf("a = %d, b = %d\n", a, b);

	return 0;
}
