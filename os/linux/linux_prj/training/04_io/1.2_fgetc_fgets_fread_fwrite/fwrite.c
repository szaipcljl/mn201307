#include<stdio.h>

struct A
{
	int a;
	char b;
	short c;
}x = {
	10,
	'a',
	100
};

int main(int argc, const char *argv[])
{

	FILE *fp;

	if((fp = fopen("./1.txt","w")) == NULL)
	{

	}

	size_t n;

	n = fwrite(&x,1,sizeof(struct A),fp);

	printf("n = %d\n",n);

	return 0;
}
