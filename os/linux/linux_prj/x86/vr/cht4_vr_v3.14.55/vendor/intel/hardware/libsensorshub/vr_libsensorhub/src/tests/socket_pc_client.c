//write by Figo. a simply socket reciver test app for ADB TCP network on Linux PC
//gcc socket_pc_client.c -o socket_pc_client -static -lrt

#include <sys/types.h>         
#include <sys/socket.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <arpa/inet.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <arpa/inet.h> 

#include <sys/time.h>
#include <time.h>
#include <unistd.h>

//#define WITH_PACKAGE_MAGIC 1
#define package_head_magic 0xabcdcbadabcdcbad
struct rotation_vector_data {
#ifdef WITH_PACKAGE_MAGIC
      int64_t magic;
#endif
	int64_t ts;
	int x;
	int y;
	int z;
	int w;
} __attribute__ ((packed));


int64_t getTimestamp()
{
        struct timespec t = { 0, 0 };
        clock_gettime(CLOCK_MONOTONIC, &t);
        return ((t.tv_sec)*1000000000 + (t.tv_nsec));
}

static int cfd;
int connect_adb_server()
{

	int recbytes;
	int sin_size;
	char buffer[1024]={0};   
	struct sockaddr_in s_add,c_add;
	unsigned short portnum=9000; 

	system("adb forward tcp:9000 tcp:9000");

	cfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == cfd)
	{
		printf("socket fail ! \r\n");
		return -1;
	}
	printf("socket ok !\r\n");

	bzero(&s_add,sizeof(struct sockaddr_in));
	s_add.sin_family=AF_INET;
	s_add.sin_addr.s_addr= inet_addr("127.0.0.1");
	s_add.sin_port=htons(portnum); 
	printf("s_addr = %#x ,port : %#x\r\n",s_add.sin_addr.s_addr,s_add.sin_port);


	if(-1 == connect(cfd,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
	{
		printf("connect fail !\r\n");
		return -1;
	}
	printf("connect ok , fd=%d\n", cfd);

	return 0;
}


int close_socket()
{
	printf("close socket\n");
	close(cfd);
}

int read_adb(char *buf, int len)
{
	int ret;
	if ((ret = recv(cfd, (void *)buf, len, 0)) < 0) {
		printf("%s fail\n", __func__);
	}
	return ret;
}

static long total= 0;
static int64_t s_time = 0;
static int invalid_package= 0;

int main()
{
	int len;
	int readnum;
	char data[100];
	struct rotation_vector_data *rvdata;
	int64_t time = 0;

retry:
	if (connect_adb_server()) {
		printf("cannot connect to adb server\n");
		return 0;
	}

	for (;;){
		readnum = read_adb(data, 100);
			if (readnum <= 0) {
			    printf("socket disconnect, re-connect...\n");
			    close_socket();
			    goto retry;	
			}
			rvdata = (struct rotation_vector_data *)data;

#ifdef WITH_PACKAGE_MAGIC
			if (rvdata->magic != package_head_magic)
#endif
			if (readnum != sizeof (struct rotation_vector_data))	 {
				//printf("======error package=========\n");
				//printf("\n");
				invalid_package++;
				continue;
			}


#if 1
			time = getTimestamp();
			if (total == 0) {
				s_time = time;
			}
			total++;
			if ((time - s_time)/1000 > 60*1000*1000) {
				printf("collect data in 1 minute %ld, invalid package %d, speed %ld ps\n", total, invalid_package, total/60);
				invalid_package = 0;
				total = 0;
			}
#endif
			
			//printf("read data len=%d  from adb: timstamp=%ld, xyzw= %d:%d:%d:%d\n", readnum, rvdata->ts, rvdata->x, rvdata->y, rvdata->z, rvdata->w);
	}

	close_socket();
}
