#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[])
{
	const char* path = "/dev/tty0";
	const char* name;

	name = strrchr(path, '/');
	if(!name)
		return -1;
	name++;

	printf("name = %s\n", name);

	return 0;
}
