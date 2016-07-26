#include <stdio.h>

int main(int argc, const char *argv[])
{
	int a = 100;
	float b = 1.456;
	long c = 20;

	double d = 2.345;

	long long f = 30;

	short e = 40;

	printf("a = %-5d\n",a);
	printf("a = %+d\n",a);

	printf("a = %05d\n",a);

	printf("a = %#o\n",a);
	printf("a = %#x\n",a);
	printf("b = %.2f\n",b);

	printf("c = %ld\n",c);
	printf("d = %lf\n",d);

	printf("f = %lld\n",f);

	printf("e = %hd\n",e);

	return 0;
}
