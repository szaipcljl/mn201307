#include<stdio.h>

int main(int argc, const char *argv[])
{
	FILE *fp;

	int fd[2];

	size_t n;
	if((fp = fopen("./1.txt","r")) == NULL)
	{

	}

	//n = fread(fd,4,2,fp);
	n = fread(fd,1,8,fp);
	printf("n = %d\n",n);
	printf("%x,%x\n",fd[0],fd[1]);
	return 0;
}
