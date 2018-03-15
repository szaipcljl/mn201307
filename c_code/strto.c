#include <stdio.h>
#include <ctype.h>

unsigned long simple_strtoul(const char *cp, char **endp,
		unsigned int base)
{
	unsigned long result = 0;
	unsigned long value;

	if (*cp == '0') {
		cp++;
		if ((*cp == 'x') && isxdigit(cp[1])) {
			base = 16;
			cp++;
		}

		if (!base)
			base = 8;
	}

	if (!base)
		base = 10;

	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
					? toupper(*cp) : *cp)-'A'+10) < base) {
		result = result*base + value;
		cp++;
	}

	if (endp)
		*endp = (char *)cp;

	return result;
}

int main(int argc, const char *argv[])
{
	int val = -1;

	val = (int)simple_strtoul(argv[1], NULL, 10);

	printf("val=%d; val=0x%x\n", val, val);

	return 0;
}
