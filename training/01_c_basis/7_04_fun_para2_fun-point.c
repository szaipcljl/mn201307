#include <stdio.h>

//#define NOT_USE_TYPEDEF

typedef int a;

#ifdef USE_TYPEDEF
int (*p_t)(int );
#else
typedef int (*p_t)(int );
#endif

int fun(int a)
{
	printf("[fun] a = %d\n",a);
	return 0;
}


int fun1(int a)
{
	printf("[fun1] print\n");
	return 0;
}

#ifdef NOT_USE_TYPEDEF
int fun2(int a,int b,int (*fp)(int a))
#else
int fun2(int a,int b, p_t fp)
#endif
{
	printf("[fun2] b = %d\t a = %d\n",b,a);

	(*fp)(a);
	return 0;
}

#ifdef NOT_USE_TYPEDEF
int (*  fun5(int a,int (*fp)(int a))   )(int)
#else
p_t fun5(int a, p_t fp)
#endif
{

	printf("fun5");
	(*fp)(a);

	return fp;
}

int main(int argc, const char *argv[])
{
	int (*fp)(int);
	int a;

	fun2(10,20,fun);
	fun2(10,20,fun1);
	fp = fun5(200,fun);

	(*fp)(300);

	return 0;
}
