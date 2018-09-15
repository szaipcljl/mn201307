#include <stdio.h>
//#include <sys/types.h>

int fun()
{
	return 0;
}


int  main(int argc, const char *argv[])
{

	int a = 10;
#if 0 //注释

	for(;;){
		break;//break an infinite loop
	}

	printf("#if compiled\n");

#else

	a = fun();
	printf("#else compiled\n");

#endif
	return 0;
}
