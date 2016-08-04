#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

int main(int argc, const char *argv[])
{
	int fd1,fd2;
	long n;
	int len;
	long size;
	char buf[64];

	fd1 = open(argv[1],O_RDONLY);
	fd2 = open(argv[2],O_WRONLY | O_CREAT | O_TRUNC,0664);

	size = lseek(fd1,0,SEEK_END);//
	size = size / 2;

	pid_t pid;

	pid = fork();

	if(pid == -1){

	}else if(pid == 0){
		//	sleep(1);
		//	n = lseek(fd1,10,SEEK_CUR);
		//	printf("n = %ld\n",n);
		//		printf("fd1 = %d,fd2 = %d\n",fd1,fd2);

		fd1 = open(argv[1],O_RDONLY);
		fd2 = open(argv[2],O_WRONLY);

		lseek(fd1,size,SEEK_SET);
		lseek(fd2,size,SEEK_SET);

		while((len = read(fd1,buf,64)) > 0)
			write(fd2,buf,len);

		close(fd1);
		close(fd2);

	}else{
		//		lseek(fd1,5,SEEK_SET);
		//		printf("fd1 = %d,fd2 = %d\n",fd1,fd2);

		lseek(fd1,0,SEEK_SET);

		while(size > 0){
			len = read(fd1,buf,64);
			write(fd2,buf,len);
			size = size - len;
		}

		wait(NULL);
		close(fd1);
		close(fd2);

	}
	return 0;
}
