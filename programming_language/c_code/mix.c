#include <stdio.h>

#define BITS_READ	8
#define VARIANTS_READ	(1 << BITS_READ)
int main(int argc, const char *argv[])
{
	int mix_i, i;

	for (i = 0; i < VARIANTS_READ; i++) {
		mix_i = ((i * 167) + 13) & 255;
		printf("%3d\t%d\n", i, mix_i);
	}
	return 0;
}
