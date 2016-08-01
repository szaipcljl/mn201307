#include<stdio.h>
#include "stack.h"

int main(int argc, const char *argv[])
{
	mstack_t *sp;
	int i;
	data_t buf;

	sp = creat_stack(6);

	for(i = 0 ; i < 10;i ++){
		push_stack(sp,&i);
	}
	print_stack(sp);

	pop_stack(sp,&buf);
	printf("pop : %d\n",buf);

	pop_stack(sp,&buf);
	printf("pop : %d\n",buf);

	pop_stack(sp,&buf);
	printf("pop : %d\n",buf);

	print_stack(sp);

	return 0;
}


