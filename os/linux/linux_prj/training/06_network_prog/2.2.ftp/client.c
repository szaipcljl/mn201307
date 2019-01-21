#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>


#include "type.h"

int download(int sockfd,send_data_t *buf)
{
	FILE *fp;

	puts("plase input a file name:");
	bzero(buf->data,sizeof(buf->data));
	fgets(buf->data,sizeof(buf->data),stdin); //a.c\n >> a.c\0  （fputs在读取\n后自动添加\0）

	buf->data[strlen(buf->data) - 1] = '\0';


	//test open file
	fp = fopen(buf->data,"w");
	if(NULL == fp)
	{
		perror("DOWNLOAD fopen:");
		return -1;
	}

	//if ok
	//send cmd DOWNLOAD
	buf->cmd = DOWNLOAD;

	if(0 >= send(sockfd,buf,sizeof(send_data_t),0))
	{
		perror("DOWNLOAD send");
		exit(-1);
	}


	//接收 确认
	if(0 >= recv(sockfd,buf,sizeof(send_data_t),0))
	{
		perror("DOWNLOAD recv cmd:");
		exit(-1);
	}

	if(SEND_START != buf->cmd)
	{
		puts("sever open file error!!!");
		return -1;
	}

	while(1)
	{
		if(sizeof(send_data_t) > recv(sockfd,buf,sizeof(send_data_t),0))
		{
			perror("DOWNLOAD recv data:");
			exit(-1);
		}
		//
		//write
		if(buf->len > fwrite(buf->data,1,buf->len,fp))
		{
			perror("DOWNLOAD fwrite data:");
			exit(-1);
		}

		if(DATA != buf->cmd)
		{
			if(DATA_END == buf->cmd)
			{
				break;
			}
			else
			{
				printf("sever fread error\n");
				return -1;
			}

		}

	}

	fclose(fp);


	return 0;
}


void print_user_list(void)
{
	printf("/****************************/\n");
	printf("/***1.download***************/\n");
	printf("/***2.getup******************/\n");
	printf("/***3.list*******************/\n");
	printf("/***4.quit*******************/\n");
	printf("/****************************/\n");

	return ;
}


int main()
{
	int tcp_socket;
	struct sockaddr_in sever_addr;
	send_data_t buf;

	//socket
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(0 > tcp_socket)
	{
		perror("socket");
		exit(-1);
	}

	sever_addr.sin_family = AF_INET;
	sever_addr.sin_addr.s_addr = inet_addr("192.168.2.43"); //服务器地址注意更换一致
	sever_addr.sin_port = htons(50000);

	if(0 > connect(tcp_socket,(SA *)&sever_addr,sizeof(sever_addr)))
	{
		perror("connect");
		exit(-1);
	}

	while(1)
	{

		print_user_list();

		//等待用户输入指令 fgets

		fgets(buf.data,sizeof(buf.data),stdin);

		buf.cmd = buf.data[0] - '0';

		switch(buf.cmd)
		{
		case DOWNLOAD:
			download(tcp_socket,&buf);
			break;
		}

	}

}
