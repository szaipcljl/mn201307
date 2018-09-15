#include <stdio.h>

#include "queue.h"
#include "stack.h"

#include "mytype.h"

int pop_and_in_queue(mstack_t *stack,queue_t *queue)
{
	data_t buf;

	while(!empty_stack(stack)){
		pop_stack(stack,&buf);
		in_queue(queue,&buf);
	}

	return 0;
}


int main()
{

	queue_t *q;
	mstack_t *min;
	mstack_t *five_min;
	mstack_t *h;
	data_t buf;

	int flag;
	int count;
	int i;

	q = creat_queue(28);

	min = creat_stack(4);
	five_min = creat_stack(11);
	h = creat_stack(11);

	for(i = 1;i < 28;i++){
		in_queue(q,&i);
	}

	count = 0;
	while(1){
		out_queue(q,&buf);
		if(! full_stack(min)){
			push_stack(min,&buf);
			continue;
		}

		pop_and_in_queue(min,q);

		if(!full_stack(five_min)){
			push_stack(five_min,&buf);
			continue;
		}
		pop_and_in_queue(five_min,q);

		if(!full_stack(h)){
			push_stack(h,&buf);
			continue;
		}
		pop_and_in_queue(h,q);
		in_queue(q,&buf);

		flag = 0;
		for(i = 1;i < 28;i ++){
			out_queue(q,&buf);
			in_queue(q,&buf);

			if(i != buf){
				flag = 1;
			}
		}

		count ++;
		printf("count = %d\t:",count);
		print_queue(q);

		if(flag == 0 ){
			break;
		}
	}

	printf("time = %d\n",count * 12);

	return 0;
}
