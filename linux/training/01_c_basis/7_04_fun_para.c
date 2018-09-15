#include <stdio.h>

int fun(int a[20])
{
	printf("sizeof(a) = %ld\n",sizeof(a));
	return 0;
}

int fun1(int a[])
{
	printf("sizeof(a) = %ld\n",sizeof(a));
	return 0;
}

int fun3(int *a)
{
	return 0;
}

int fund(int (*p)[4])
{
	printf("sizeof(p) = %ld\n",sizeof(p));
	return 0;
}

//int fund(int *p,int v,int l);//error: conflicting types for ‘fund’

int fund3(int (*p)[2][2]);


int main(int argc, const char *argv[])
{
	int a[128];

	int b[4][4];
	int c[2][2][2];

	fun(a);
	fun1(a);

	fund(b);

	return 0;
}
