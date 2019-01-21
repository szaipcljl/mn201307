#include<stdio.h>
#include<errno.h>

int main(int argc, const char *argv[])
{
	FILE *fp;
	char buf[100];
	//	if((fp = fopen("./1.txt","r")) == NULL)
	if((fp = fopen("./1.txt","r+")) == NULL)
	{
		//	printf("error\n");
		perror("fopen12345");
		printf("%d\n",errno);
		return -1;
	}
#if 0
	fscanf(fp,"%s",buf);

	printf("%s\n",buf);

	fscanf(fp,"%s",buf);
	printf("%s\n",buf);
#endif

	fprintf(fp,"hello");
	fprintf(fp,"world");

	fclose(fp);
	return 0;
}
