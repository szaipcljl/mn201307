#include <stdio.h>

void fun(void)
{

}

int main(int argc, const char *argv[])
{
	int a[4][4];
	int b[4];
	int *p = b;
	int **pp;
	pp = a;
	int (*ap)[4];

	ap = a;

	printf("ap = %p\n",ap);
	printf("ap + 1 = %p\n",ap + 1);

	printf("pp = %p\n",pp);
	printf("pp + 1 = %p\n",pp + 1);

	a[2][2] = 33;
	//*(*(a + 2) + 2);
	printf("*(*(a + 2) + 2) = %d\n",*(*(a + 2) + 2));
	printf("*(*(ap + 2) + 2) = %d\n",*(*(ap + 2) + 2));
	printf("*(*(pp + 2) + 2) = %d\n",*(*(pp + 2) + 2));

#if 0
	printf("%d\n",a);
	printf("%d\n",&a[0]);
	printf("%d\n",a[0]);
	printf("%d\n",&a[0][0]);
#endif
	return 0;
}
