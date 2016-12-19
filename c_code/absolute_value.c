#include <stdio.h>

#define ABS_VAL(x) (x > 0 ? x:(-x))

void abs_max_val_reassign(int *x, int *y, int *z)
{
	int a = *x, b = *y, c = *z; 
	if (ABS_VAL(a) > ABS_VAL(b)) {
		if (ABS_VAL(a) > ABS_VAL(c))
			a > 0 ? (*x = 45000) : (*x = -45000);
		else
			c > 0 ? (*z = 45000) : (*z = -45000);
	} else {
		if (ABS_VAL(b) > ABS_VAL(c))
			b > 0 ? (*y = 45000) : (*y = -45000);
		else
			c > 0 ? (*z = 45000) : (*z = -45000);
	}
}

int main(int argc, const char *argv[])
{
	int a = 99, b = 144, c = -155;

	printf("a = %d\tb = %d\tc = %d\n", a, b, c);

	abs_max_val_reassign(&a,&b,&c);

	printf("a = %d\tb = %d\tc = %d\n", a, b, c);

	return 0;
}
