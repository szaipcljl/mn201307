#include <stdio.h>

#define notrace __attribute__((no_instrument_function))
#define _RET_IP_    (unsigned long)__builtin_return_address(0)

void notrace __sanitizer_cov_trace_pc(void)
{
	//printf("code instrumented...\n");
	printf("return pc [0x%x]\n", _RET_IP_);
}

