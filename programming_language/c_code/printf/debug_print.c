#include <stdio.h>

#define DEBUG 3
//#define DEBUG 0

#if defined(DEBUG) && DEBUG > 0
#define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
		__FILE__, __LINE__, __func__, ##args)
#else
#define DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
#endif

int main(int argc, char **args)
{
	DEBUG_PRINT("Debugging is enabled.\n");
	DEBUG_PRINT("Debug level: %d\n", (int) DEBUG);
}
