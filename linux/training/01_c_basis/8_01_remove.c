#include <stdio.h>

int main(int argc, const char *argv[])
{
	char filename[80];

	printf("The file to delete:");
	gets(filename);

	if(remove(filename) == 0)
		printf("Removed %s.\n", filename);
	else
		perror("remove");
}
