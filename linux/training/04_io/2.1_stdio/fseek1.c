#include<stdio.h>

int main(int argc, const char *argv[])
{
	FILE *fp;

	fp = fopen("./2.txt","w");

	fseek(fp,10,SEEK_SET);

	fputc('a',fp);
	return 0;
}
