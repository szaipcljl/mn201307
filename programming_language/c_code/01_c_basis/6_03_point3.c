#include <stdio.h>

/*
 * 关于段错误：
 * 1.野指针  一定要杜绝
 * 2.空指针  (void * )0  NULL
 * 3.栈区溢出
 * 4.text段
 * */


int main(int argc, const char *argv[])
{
	int a = 9,a1,a2;
	const int ca = 100;
	const int *constp = &a;
	int * const constp2 = &a;
	const int *const constconstp = &a;
	char *cp = NULL;
	int b[128];
	int *p = &a,*p10 = NULL,*p11 = &a1;  //指针声明时，* 不可省略
	int *p1 = &b[0];
	int *p2 = &b[120];
	int *(*pp) = &p;
	int ***ppp = &pp;
	*p = 120;
	cp = p;
	//ca = 120;

#if 0
	constp = &a1;
	*constp = 128;
	constp2 = &a1;
	*constp2 = 128;
	constconstp = &a1;
	*constconstp = 123;
#endif

	printf("(*p): a = %d\n",a);
	**pp = 130;
	printf("(**pp): a = %d\n",a);

	p10 = (int *)&p;
	///	ppp = NULL;
	***ppp = 1234567;
	printf("(***ppp): a = %d\n",a);

	printf("p1 - p2 = %ld\n",p1 - p2); //中间相差的元素的个数
#if 0
	//error: invalid operands to binary + (have ‘int *’ and ‘int *’)

	printf("p1 + p2 = %ld\n",p1 + p2);
	printf("p1 * p2 = %ld\n",p1 * p2);
	printf("p1 / p2 = %ld\n",p1 / p2);
#endif

	p1[9] = 234;

	printf("b[9] = %d\n",b[9]);
	printf("*(b + 9) = %d\n",*(b + 9));
	b[99] = 333;
	b[100] = 111;
	b[101] = 222;
	*(p2 - 20) = 100000;

	printf("b[100] = %d\n",b[100]);

	return 0;
}
