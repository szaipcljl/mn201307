#include<stdio.h>

int main(int argc, const char *argv[])
{
	FILE *fp;

	char buf[100];
	//if((fp = fopen("./1.txt","a")) == NULL)

	if((fp = fopen("./1.txt","a+")) == NULL)
	{
		perror("fopen");
		return -1;
	}


	//fprintf(fp,"123456789");

	//rewind(fp);
	fscanf(fp,"%s",buf);
	printf("%s\n",buf);

	fprintf(fp,"abcdef");
	return 0;
}
