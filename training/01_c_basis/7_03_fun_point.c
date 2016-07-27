#include <stdio.h>

int fun (int a,int b)
{
	printf("[fun] print\n");
	return  0;
}



int main(int argc, const char *argv[])
{
	int (*p)(int a,int b ); //函数指针变量：存放函数的地址，
	//<数据类型> (*<函数指针名称>)（<参数说明列表>）;

	p = fun; //函数的入口地址存放到指针P中，

	(*p)(1,2); //调用指针（入口地址）指向到函数

	return 0;
}
