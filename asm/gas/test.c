#include <stdio.h>

extern unsigned long long return_rax(void);
asm(
		".section .text\n"
		".global return_rax\n"
		"return_rax:\n"
		"	movq $0x8, %rax\n"
		"	ret\n"
   );
// 64bit: return %rax 32bit: return %eax


void test()
{
	int a=10, b;
	asm ( "movl %1, %%eax;"
			"movl %%eax, %0;"
			:"=r"(b)           /* output */
			:"r"(a)              /* input */
			:"%eax"         /* clobbered register */
		);

	printf("b = %d\n", b);
}


int main(int argc, const char *argv[])
{
	unsigned long long ret;

	ret = return_rax();
	printf("ret = %llx\n", ret);

	test();

	return 0;
}
