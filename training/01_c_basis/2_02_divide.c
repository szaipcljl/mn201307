#include <stdio.h>

int main(int argc, const char *argv[])
{

#if 1
	int a = 10, b = 20;
	float c;

	c = a / b;
	printf("a = %d, b = %d, c = %f\n",a,b,c);
#else

	int a = 10, b = 20;
	int *p = &a;
	int *q = &b;
	int c;

	c = *p / *q;
	printf("*p = %d, *q = %d,c = %d\n",*p,*q,c);
#endif

	return 0;
}
