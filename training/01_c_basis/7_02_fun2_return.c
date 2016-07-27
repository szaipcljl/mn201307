#include <stdio.h>

int *fun(int *p)
{
	int a = 100;
	return &a;     //警告：一个局部变量的地址不可以作为返回值
	//warning: function returns address of local variable
}

int fun2(int *p)
{
	int a[8]= {0,1,2,3,4,5,6,7};
	return 0;
}


int main(int argc, const char *argv[])
{
	int *p;

	p = fun(NULL);
	printf("%d\n",*p);

	fun2(p);
	printf("%d\n",*p);

	return 0;
}


