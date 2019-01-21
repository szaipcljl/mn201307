#include<stdio.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<semaphore.h>
pthread_cond_t cond;
pthread_mutex_t mutex;
void *read_file(void *arg)
{
	int fd;
	int n;
	char buf[64];
	fd = open((const char *)arg,O_RDONLY | O_CREAT | O_TRUNC,0664);

	while(1)
	{
		pthread_mutex_lock(&mutex);
	//	pthread_cond_wait(&cond,&mutex);

		while((n = read(fd,buf,sizeof(buf) -1)) == 0)
			pthread_cond_wait(&cond,&mutex);

		buf[n] = '\0';

		printf("read the string %s",buf);
		pthread_mutex_unlock(&mutex);
	}
}

void *write_file(void *arg)
{
	int fd;
	char buf[64];
	int n;
	fd = open((const char *)arg,O_WRONLY | O_TRUNC | O_CREAT,0664);

	while(1)
	{
		usleep(500);
		pthread_mutex_lock(&mutex);
		n = read(0,buf,sizeof(buf));

		if(strncmp(buf,"quit",4) == 0)
			break;
		write(fd,buf,n);
		pthread_mutex_unlock(&mutex);
		pthread_cond_signal(&cond);
	}
	close(fd);
}

int main(int argc, const char *argv[])
{
	int ret;

	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&cond,NULL);
	pthread_t tid[2];

	ret = pthread_create(&tid[0],NULL,read_file,(void *)argv[1]);
	ret = pthread_create(&tid[1],NULL,write_file,(void *)argv[1]);

	pthread_join(tid[1],NULL);
	return 0;
}
