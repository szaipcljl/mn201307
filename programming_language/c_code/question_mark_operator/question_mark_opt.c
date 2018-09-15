#include <stdio.h>

void test(int *p)
{
	p? *p = 3 : 0;
	p? printf("*p = %d\n", *p) : printf("p = NULL\n");
}

int main(int argc, const char *argv[])
{
	int a = 2;
	int flag = 1;

	a = flag? 10:0;	
	printf("a= %d\n", a);

	test(NULL);
	test(&a);

	return 0;
}
