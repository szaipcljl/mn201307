#include<stdio.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
int count = 0;
pthread_mutex_t mutex;
void *fun1(void *arg)
{
	int fd = *(int *)arg;
	while(1)
	{
		pthread_mutex_lock(&mutex);
		if(count == 0)
		{
			count ++;
			write(fd,"hello\n",6);
		}
		pthread_mutex_unlock(&mutex);
	}
}

void *fun2(void *arg)
{
	int fd = *(int *)arg;
	while(1)
	{
		pthread_mutex_lock(&mutex);
		if(count > 0)
		{
			count --;
			write(fd,"abc\n",4);
		}
		pthread_mutex_unlock(&mutex);
	}
}

int main(int argc, const char *argv[])
{
	int ret;

	int fd;

	fd = open("./1.txt",O_RDWR | O_CREAT | O_TRUNC,0664);
	pthread_t tid[2];

	pthread_mutex_init(&mutex,NULL);
	ret = pthread_create(&tid[0],NULL,fun1,(void *)&fd);
	ret = pthread_create(&tid[1],NULL,fun2,(void *)&fd);

	while(1);
	return 0;
}
