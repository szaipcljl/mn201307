#include <stdio.h>
#undef scanf

#define E 2.71828
int e_squared = E * E;
#undef E

/*
 * Usually, #undef is used to scope a preprocessor constant into a very limited
 * region--this is done to avoid leaking the constant (for example, changing a
 * single letter like E would be dangerous across a large program, but in a
 * short scope, it is comparatively safe. #undef is the only way to create this
 * scope since the preprocessor does not understand block scopes defined with
 * { and }.
 */

int scanf(const char * s, ...)
{
	printf(s);
	return 0;
}

int main()
{
	scanf("hello\n");
	printf("e_squared=%d\n", e_squared);
	//printf("E=%d\n", E); //error: ‘E’ undeclared (first use in this function)
	return 0;
}
