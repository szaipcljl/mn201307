#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
	printf("[%s]: run an application in c program\n", __FILE__);
	system("./cshell");
	return 0;
}
