#include<stdio.h>

int main(int argc, const char *argv[])
{
	char buf[] = "hello";
	char str[100];
	printf("%s\n",buf);
	fprintf(stdout,"%s\n",buf);


	sprintf(str,"world\n");
	printf("%s",str);
	return 0;
}
