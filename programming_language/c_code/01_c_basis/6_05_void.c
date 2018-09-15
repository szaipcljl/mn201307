#include <stdio.h>

int main(int argc, const char *argv[])
{
	void *p,*p2;
	int a;
	char *cp;

	cp = &a; //warning: assignment from incompatible pointer type (从不相容的指针类型赋值)
	p = &a;
	cp = p;

	p ++;
	cp ++;
	printf("p = %p\n",p);
	printf("++p = %p\n",++p);
	printf("cp = %p\n",cp);
	printf("++cp = %p\n",++cp);


	p - p2;
	//p + p2; //error: invalid operands to binary + (have ‘void *’ and ‘void *’)
	//*p = 23; //error: invalid use of void expression

	return 0;
}
