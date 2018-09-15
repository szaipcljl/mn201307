#include <stdio.h>
#include <unistd.h>
#include "circ_buf.h"
#include <pthread.h>

#define SIZE (1 << 4)	//16
#define PRD_NUM (100 * 10000)
#define CSM_NUM (100 * 10000)
//producer-consumer model

void* data_producer(void* arg)	//producer 
{
	int i, ret, data_len;
	char value ;
	circ_buf_t producer_buf;
	struct my_fifo* fifo_p = (struct my_fifo*) arg;

	printf("%s:%d\n", __func__, __LINE__);

	for (i = 0; i < PRD_NUM; i++) {
		value = i % 255;
		producer_buf.a = value;
		producer_buf.b = i;
		producer_buf.my_vector.x = i+1;
		producer_buf.my_vector.y = i+2;
		producer_buf.my_vector.z = i+3;

		ret = myfifo_in(fifo_p, &producer_buf, 1);
		data_len = myfifo_used(fifo_p);

		printf("@@##:%s:%d:\ta = %6d,\tb = %6d,\tproducer_buf.my_vector.x = %6d,"
				"y = %6d, z = %6d,\twrite ret = %d,\tdata_len = %4d\n", __func__, __LINE__,
				producer_buf.a, producer_buf.b,\
				producer_buf.my_vector.x, producer_buf.my_vector.y,\
				producer_buf.my_vector.z, ret,data_len);

		if(data_len == (SIZE-1)) //for test
			myfifo_reset(fifo_p);

		usleep(100 *1000);//for test
	}

}

int main(int argc, const char *argv[])
{
	printf("%s,%d\n", __func__, __LINE__);

	int ret;
	int *p;
	pthread_t tid;
	struct my_fifo * pdata_fifo;

	pdata_fifo = malloc(sizeof(struct my_fifo));

	ret = myfifo_alloc(pdata_fifo, SIZE, sizeof(circ_buf_t));
	if (ret) {
		printf("failed to alloc my_fifo.\n");
		return -1;
	}

	ret = pthread_create(&tid, NULL, data_producer, (void*)pdata_fifo);
	if (ret < 0) {
		return -1;	
	} 

	int i;
	int data_len;
	circ_buf_t consumer_buf;

	myfifo_reset(pdata_fifo);

	//consumer
	for (i = 0; i < CSM_NUM; i++) {
		while (!(ret = myfifo_out(pdata_fifo, &consumer_buf, 1)));
		data_len = myfifo_used(pdata_fifo);

		printf("@@##:%s:%d:\t\ta = %6d,\tb = %6d,\tconsumer_buf.my_vector.x = %6d,"
				"y = %6d, z = %6d,\tread  ret = %d,\tdata_len = %4d\n", __func__, __LINE__,\
				consumer_buf.a, consumer_buf.b,\
				consumer_buf.my_vector.x, consumer_buf.my_vector.y,\
				consumer_buf.my_vector.z, ret, data_len);

		if(data_len == (SIZE-1))//for test
			myfifo_reset(pdata_fifo);

		usleep(500 *1000);//for test
	}

	pthread_join(tid,NULL);
	return 0;
}
