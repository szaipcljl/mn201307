#include <stdio.h>
#include <stdlib.h>

/*存在 BTB 失效的例子程序*/
void foo()
{
	int i,j;
	for(i=0; i< 10; i++)
		j+=2;
}

int main(void)
{
	int i;
	for(i = 0; i< 100000000; i++)
		foo();
	return 0;
}
