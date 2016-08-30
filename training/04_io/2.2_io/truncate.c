#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main(int argc, const char *argv[])
{
	truncate("./3.txt",10);
	return 0;
}
