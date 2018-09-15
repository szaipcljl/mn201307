#include <stdio.h>

#define DEBUG //switch

#ifdef DEBUG
#define _DEBUG	1
#else
#define _DEBUG	0
#endif

#ifndef pr_fmt
#define pr_fmt(fmt) fmt
#endif

/*
 * Output a debug text when condition "cond" is met. The "cond" should be
 * computed by a preprocessor in the best case, allowing for the best
 * optimization.
 */
#define debug_cond(cond, fmt, args...)			\
	do {						\
		if (cond)				\
			printf(pr_fmt(fmt), ##args);	\
	} while (0)

#define debug(fmt, args...)			\
	debug_cond(_DEBUG, fmt, ##args)

int main(int argc, const char *argv[])
{
	debug("#mn:this style debug is copied from uboot2016.1[include/common.h] func: %s\n", __func__);
	return 0;
}
