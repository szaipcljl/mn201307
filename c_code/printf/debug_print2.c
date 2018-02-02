#include <stdio.h>

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT printf
#else
#define DEBUG_PRINT
#endif

int main(int argc, char **args)
{
	DEBUG_PRINT("Debugging is enabled.\n");
}
