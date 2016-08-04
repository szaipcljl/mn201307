#include<stdio.h>
#include<unistd.h>

int main(int argc, const char *argv[])
{
	printf("hello\n");
	fork();
	fork();

	fork();
	while(1);
	return 0;
}
