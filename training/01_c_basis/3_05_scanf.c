#include <stdio.h>

int main(int argc, const char *argv[])
{
	int a;
	int b;

	printf("input \'a!b\':\n");
	scanf("%d!%d",&a,&b);
	printf("%d,%d\n",a,b);

	printf("input a:\n");
	scanf("%i",&a);
	printf("%d\n",a);

	return 0;
}
