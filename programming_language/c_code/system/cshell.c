#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
	printf("[%s]: run a script in c program\n", __FILE__);
	system("./test.sh");
	return 0;
}
