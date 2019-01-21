#include<stdio.h>

int main(int argc, const char *argv[])
{
	FILE *fp;
	char buf[100];
	//	if((fp = fopen("./1.txt","w")) == NULL)

	if((fp = fopen("./1.txt","w+")) == NULL)
	{
		perror("fopen");
		return -1;
	}

	fprintf(fp,"hello");

	rewind(fp);

	//	fprintf(fp,"a");

	/*
	 * w没有读权限
	 * */
	fscanf(fp,"%s",buf);
	printf("%s\n",buf);
	return 0;
}
