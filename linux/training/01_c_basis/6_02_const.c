#include <stdio.h>

//const int temp = 100;
#define temp 100

int main(int argc, const char *argv[])
{
	const int a = 100;
	int *p = &a; //warning: initialization discards qualifiers from pointer target type

	printf("a = %d\n",a);

	*p = 200;
	printf("a = %d\n",a);

	//a = temp; //error: assignment of read-only variable ‘a’

	return 0;
}
