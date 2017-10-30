#include <stdio.h>

/*
 * nma@linux-d:~/mn/mn201307/debug/gdb[master]$ gcc -g core_dump_test.c 
 *
 * nma@linux-d:~/mn/mn201307/debug/gdb[master]$ ./a.out 
 * Segmentation fault (core dumped)
 * nma@linux-d:~/mn/mn201307/debug/gdb[master]$ ls
 * a.out  core_dump_test.c
 * nma@linux-d:~/mn/mn201307/debug/gdb[master]$ ulimit -c
 * 0
 * nma@linux-d:~/mn/mn201307/debug/gdb[master]$ ulimit -c unlimited
 * nma@linux-d:~/mn/mn201307/debug/gdb[master]$ ./a.out 
 * Segmentation fault (core dumped)
 * nma@linux-d:~/mn/mn201307/debug/gdb[master]$ ls
 * a.out  core  core_dump_test.c
 *
 * nma@linux-d:~/mn/mn201307/debug/gdb[master]$ gdb a.out core
 */

char *str = "test";

void core_test()
{
	str[1] = 'T';
}

int main()
{
	core_test();

	return 0;
}
