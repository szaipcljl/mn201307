#include<stdio.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<semaphore.h>

sem_t sem_write,sem_read;

void *read_file(void *arg)
{
	int fd;
	int n;
	char buf[64];
	fd = open((const char *)arg,O_RDONLY | O_CREAT | O_TRUNC,0664);

	while(1)
	{
		sem_wait(&sem_read);
		n = read(fd,buf,sizeof(buf) -1);

		buf[n] = '\0';

		printf("read the string %s",buf);
		sem_post(&sem_write);
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
		sem_wait(&sem_write);
		n = read(0,buf,sizeof(buf));

		if(strncmp(buf,"quit",4) == 0)
			break;
		write(fd,buf,n);
		sem_post(&sem_read);

	}
	close(fd);
}

int main(int argc, const char *argv[])
{
	int ret;


	sem_init(&sem_write,0,1);
	sem_init(&sem_read,0,0);
	pthread_t tid[2];

	ret = pthread_create(&tid[0],NULL,read_file,(void *)argv[1]);
	ret = pthread_create(&tid[1],NULL,write_file,(void *)argv[1]);

	pthread_join(tid[1],NULL);
	return 0;
}
