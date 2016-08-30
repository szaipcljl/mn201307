#include<stdio.h>

int main(int argc, const char *argv[])
{
	FILE *fp;

	char buf[10];

	if((fp = fopen("./1.txt","r")) == NULL)
	{

	}

	fgets(buf,sizeof(buf),fp);

	printf("%s\n",buf);
	return 0;
}
