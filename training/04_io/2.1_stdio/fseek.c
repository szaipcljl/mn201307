#include<stdio.h>

int main(int argc, const char *argv[])
{
	FILE *fp;

	if((fp = fopen("./1.txt","w+")) == NULL)
	{

	}
	//	printf("%p\n",fp);
	fseek(fp,10,SEEK_SET);
	fputc('a',fp);
	printf("%ld\n",ftell(fp));
	//	printf("%p\n",fp);

	fseek(fp,5,SEEK_END);
	fputc('b',fp);
	printf("%ld\n",ftell(fp));

	fseek(fp,3,SEEK_CUR);
	fputc('c',fp);

	fseek(fp,-4,SEEK_END);
	fputc('d',fp);
	fputc('e',fp);
	return 0;
}
