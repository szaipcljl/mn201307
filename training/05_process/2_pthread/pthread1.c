#include <stdio.h>
#include <pthread.h>

int global = 100;

void *fun1(void *arg)
{
	while(1)
	{
		printf("global:%d\n",global);
		sleep(1);
	}
}

void *fun2(void *arg)
{
	int value = *(int *)arg;

	static int i;
	static int a = 200;
	for(i = 0;i < 5;i ++)
	{
		printf("global:%d,value:%d\n",global,value);
		sleep(1);
	}

	pthread_exit((void *)&a);
}

int main(int argc, const char *argv[])
{
	int ret;
	int value = 10;
	int *p;
	pthread_t tid[2];

	ret = pthread_create(&tid[0],NULL,fun1,NULL);

	if(ret == -1){

	}

	ret = pthread_create(&tid[1],NULL,fun2,(void *)&value);

	if(ret == -1){

	}
	//	pthread_cancel(tid[1]);
	//	while(1);

	//	pthread_join(tid[1],NULL);

	pthread_join(tid[1],(void **)&p);
	printf("%d\n",*p);

	return 0;
}
