#include <stdio.h>

int main(int argc, const char *argv[])
{
	printf("test\n");

#ifdef POISON
	printf("%s is defined\n", "POISON");
#endif

#ifdef VISUALIZE
	printf("%s is defined\n", "VISUALIZE");
#endif

#ifdef RANDOMIZE
	printf("%s is defined\n", "RANDOMIZE");
#endif

	return 0;
}
