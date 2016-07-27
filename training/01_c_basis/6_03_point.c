#include <stdio.h>

int main(int argc, const char *argv[])
{
	int a = 11,b = 22;
	int c[] = {0,1,2,3,4,5,6,7,8,9};

	int *pa = &a;

	int *pb;
	pb = &b;

	int *pc = &c[0];

	printf("pa = %ld pb = %d\n",(long)pa,(int)pb); //64bit adress use long int;对比后面的int看运行结果
	printf("pa = %ld pb = %d\n",(long)&a,(int)&b);

	printf("*p = %d *pb = %d\n",*pa,*pb);
	printf("*pc = %d *(pc + 1) = %d pc[6] = %d\n",*pc,*(pc + 1),pc[6]);

	return 0;
}
