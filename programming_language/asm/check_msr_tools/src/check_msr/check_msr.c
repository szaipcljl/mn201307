#include <stdio.h>

#define debug
#ifndef debug
#undef printf
#define printf(fmt, args...)
#endif

int check_msr()
{
	int ret;

	__asm__ (
			"mov $0x07, %%eax\n\t"
			"mov $0x0, %%ecx\n\t"
			"cpuid\n\t"
			"mov %%edx, %0\n\t"
			: "=r" (ret) /*output*/
			: "0" (0) /*input*/
			: "%eax", "%ecx", "%ebx", "%edx");

	printf("edx = %d\n", ret);
	ret = (ret >> 29) & 1;
	printf("edx[29] = %d\n", ret);

	return ret;
}

int main(int argc, const char *argv[])
{
	int ret;
	ret = check_msr();

	return ret;
}
