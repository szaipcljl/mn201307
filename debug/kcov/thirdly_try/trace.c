#include <stdio.h>
#include "trace.h"

void notrace __sanitizer_cov_trace_pc(void)
{
	printf("code instrumented...\n");
}
