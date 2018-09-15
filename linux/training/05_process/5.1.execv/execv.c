#include <stdio.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	char* const path = (char* const)"/bin/sh";
	char* const args[] = { path, NULL };

	printf("execv(%s)\n", path);
	execv(path, args);
	return 0;
}
