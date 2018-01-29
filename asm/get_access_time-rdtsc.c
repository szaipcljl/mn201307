#include <stdio.h>
#include <string.h>

#define TARGET_OFFSET	12
#define TARGET_SIZE	(1 << TARGET_OFFSET)
#define BITS_READ       8
#define VARIANTS_READ	(1 << BITS_READ)

static char target_array[VARIANTS_READ * TARGET_SIZE];

static inline int get_access_time(volatile char *addr)
{
	register unsigned cyc_high1, cyc_low1, cyc_high2, cyc_low2;
	volatile int j;

	asm volatile ("cpuid\n\t"
			"rdtsc\n\t"
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t"
			: "=r" (cyc_high1), "=r" (cyc_low1):: "%rax", "%rbx", "%rcx", "%rdx");

	j = *addr;

	asm volatile("RDTSCP\n\t"
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t"
			"CPUID\n\t": "=r" (cyc_high2), "=r"
			(cyc_low2):: "%rax", "%rbx", "%rcx", "%rdx");

	return cyc_low2 - cyc_low1;
}


int main(int argc, const char *argv[])
{
	int time;
	volatile char *addr;

	memset(target_array, 1, sizeof(target_array));
	addr = &target_array[255 * TARGET_SIZE];

	time = get_access_time(addr);
	printf("time = %d\n", time);
	return 0;
}
