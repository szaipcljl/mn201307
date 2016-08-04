#include<stdio.h>
#include<signal.h>

int main(int argc, const char *argv[])
{
	raise(SIGKILL);
	while(1);
	return 0;
}
