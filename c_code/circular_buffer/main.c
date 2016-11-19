#include <stdio.h>
#include <unistd.h>
#include "circ_buf.h"
#include <pthread.h>

#define SIZE (1 << 8)	//256
#define PRD_NUM (100 * 10000)
#define CSM_NUM (100 * 10000)
//producer-consumer model

void* data_producer(void* arg)	//producer 
{
	int i, ret;
	unsigned char value; //0~255
	struct my_fifo* fifo_p = (struct my_fifo*) arg;

	printf("%s:%d\n", __func__, __LINE__);

	for (i = 0; i < PRD_NUM; i++) {
		value = i % 255;
		ret = my_fifo_put(fifo_p, (unsigned char *)&value, 1);

		printf("@@##:%s:%d:\ti = %d,\tret = %d\n", __func__, __LINE__, value, ret);

		usleep(3*1000);
	}

}

int main(int argc, const char *argv[])
{
	printf("%s,%d\n", __func__, __LINE__);

	int ret;
	int *p;
	pthread_t tid;
	struct my_fifo * test_fifo;

	test_fifo = my_fifo_alloc(SIZE);
	if (!test_fifo) {
		printf("failed to alloc my_fifo.\n");
		return -1;
	}

	ret = pthread_create(&tid, NULL, data_producer, (void*)test_fifo);
	if (ret < 0) {
		return -1;	
	} 

	int i;
	unsigned char value;

	my_fifo_reset(test_fifo);

	//consumer
	for (i = 0; i < CSM_NUM; i++) {
		while (!(ret = my_fifo_get(test_fifo, (unsigned char *)&value, 1)));

		printf("@@##:%s:%d:\tvalue = %d,\tret = %d\n", __func__, __LINE__, value, ret);

	}

	pthread_join(tid,NULL);
	return 0;
}
