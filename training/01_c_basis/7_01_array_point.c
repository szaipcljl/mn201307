#include <stdio.h>

/*
 * 1.不同类型的指针之间不能相互赋值
 * 2.指针变量之间的计算是受限制的： 不能做 + * \
 * 3.void * 指针变量 可以与其他任意类型指针相互赋值
 * 4.通过void * 指针变量 所引用的内存块 不能进行任何操作
 * 5.空指针 NULL  和野指针 指向的内存 不可以进行任何操作
 *     理想状态下会产生段错误,非理想状态错误不确定
 *     在初始化指针变量时一定要指定值 或 NULL
 * 6.void 不是一种变量类型 代表什么都没有
 * 7.一维数组和 一级指针 书写格式可以互换 但,仅仅限一维数组
 * */

int main(int argc, const char *argv[])
{

#if 0
	int a[][5] = {"123","456"};
#else
	int *a[5] = {"123","456"};
#endif

	int b;
	int *p = NULL; //指针p指向零值,赋值防止野指针出现
	p = &b;

	*p = 12; // >> b = 12

	int  c[2][4];
	int **pp;
	//pp = (int **)c;
	//**pp **c 最终所引用的对象完全不同
	// **c ： c[0][0]
	// **pp : *(int *)c[0][0]
	int (*cp)[4];
	cp = c;
	**cp == **c;

	//--------------------------
	int d[128];
	int *dp = &d;

	int i;
	for(i = 0;i < 64;i ++){
		printf("%-8d ",*(dp + 2 * i + 1));
		if(0 == i % 16)
			putchar(10);
	}
	printf("%d\n",d[0]);




	int *ep[12];
	int e[4][4];
	ep[1] = &e[0];
	ep[2] = &e[1];
	ep[3] = &e[2];


	e[0][1] = ep[1][1];

	const int f = 100;

#define a 100;
	const int *fp; //起到数据保护

	int g[4][4];

	/*
sizeof : a : int [4][4]
a + 1  : a : int (*)[4]   :&a[0]

sizeof :a[0]: 一行数组 的名称
a[0]+1 :a[0]: int * ;     :&a[0][0]


*/


	return 0;
}
