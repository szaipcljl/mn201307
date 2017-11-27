#include <stdio.h>

int is_empty(int i)
{
	printf("i = %d\n", i);
	return i;
}

int main(int argc, const char *argv[])
{
	int i = 1;
	int j = 0;

	is_empty(i);
	printf("hello word! j = %d\n", j);
	return 0;
}
