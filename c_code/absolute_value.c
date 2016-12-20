#include <stdio.h>

#define ABS_VAL(x) (x > 0 ? x:(-x))

#define REASSIGN_ABS_VAL 46000
void max_abs_val_reassign(int *x, int *y, int *z)
{
	int a = *x, b = *y, c = *z;
	if (ABS_VAL(a) > ABS_VAL(b)) {
		if (ABS_VAL(a) > ABS_VAL(c))
			a > 0 ? (*x = REASSIGN_ABS_VAL) : (*x = -1 * REASSIGN_ABS_VAL);
		else
			c > 0 ? (*z = REASSIGN_ABS_VAL) : (*z = -1 * REASSIGN_ABS_VAL);
	} else {
		if (ABS_VAL(b) > ABS_VAL(c))
			b > 0 ? (*y = REASSIGN_ABS_VAL) : (*y = -1 * REASSIGN_ABS_VAL);
		else
			c > 0 ? (*z = REASSIGN_ABS_VAL) : (*z = -1 * REASSIGN_ABS_VAL);
	}
}

int main(int argc, const char *argv[])
{
	int a = 99, b = 144, c = -155;

	printf("a = %d\tb = %d\tc = %d\n", a, b, c);

	max_abs_val_reassign(&a,&b,&c);

	printf("a = %d\tb = %d\tc = %d\n", a, b, c);

	return 0;
}
