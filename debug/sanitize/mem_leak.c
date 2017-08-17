#include <stdio.h>
#include <stdlib.h>
/*
 * memory leak:
 * allocated memory and forgot to free it later.
 *
 * Notice the call to malloc that allocates a kilobyte of memory.
 * There is no accompanying call to free and so the kilobyte of
 * memory is “lost” and only recovered when the program ends.
 */
int main(int argc, char** argv)
{
	char * buffer = malloc(1024);
	sprintf(buffer, "%d", argc);
	printf("%s",buffer);
}
