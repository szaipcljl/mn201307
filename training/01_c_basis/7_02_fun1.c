#include <stdio.h>

//gcc 7_02_fun1.c 7_02_fun1-otherfun_.c

int add( int *a,int b)
{
	return *a + b;
}

int add2(const int *a,int b)
{
	int c;
	c = *a + b ;
	a = (int *)20000; //a指向到对象不能改变，add2()创建指针变量a，
	//	存a的地址，主函数a的值通过传递到地址值操作，直接改变add2（）创建
	//	的a中的值，主函数中a的值不变。
	return c;
}

int fun(void)
{
	static int a = 0;
	a ++;
	return a;
}

extern void otherfun(void);

int main(int argc, const char *argv[])
{
	int a;
	int ans;
	int *p = &a;
	a = 100;

	ans = add(&a,20);
	printf("[add] ans = %d  a = %d\n",ans,a);


	ans = add2(p,20);
	printf("[add2] ans = %d  a = %d\n",ans,a);



	printf("[fun] return: %d \n",fun());
	printf("[fun] return: %d \n",fun());
	printf("[fun] return: %d \n",fun());
	printf("[fun] return: %d \n",fun());

	otherfun();

	return 0;
}
