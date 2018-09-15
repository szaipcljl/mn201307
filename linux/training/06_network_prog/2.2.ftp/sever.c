#include <stdio.h>
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


	// test open file
	fp = fopen(buf->data,"r"); //编译时在同一个文件夹编译，搜索头文件；编译后生成到sever
	if(NULL == fp) 				//复制到文件夹file中，下载赋值file中文件到client所在文件夹
	{ 							//避免同一个文件夹中重名出错
		perror("DOWNLOAD fopen");
		buf->cmd = DATA_ERROR;
		if(0 >= send(sockfd,buf,sizeof(send_data_t),0))
		{
			perror("DOWNLOAD send1:");
			exit(-1);
		}
		return -1;
	}
	//  1.打开成功
	//  2.打开失败
	//    send 》》 ok buok

	buf->cmd = SEND_START;
	if(0 >= send(sockfd,buf,sizeof(send_data_t),0))
	{
		perror("DOWNLOAD send2:");
		exit(-1);
	}

	while(1)
	{
		//read
		buf->len = fread(buf->data,1,sizeof(buf->data),fp);
		if(sizeof(buf->data) > buf->len)
		{
			if(feof(fp))
			{
				buf->cmd = DATA_END;
			}
			else
			{
				buf->cmd = DATA_ERROR;
			}

			if(0 >= send(sockfd,buf,sizeof(send_data_t),0))
			{
				perror("DOWNLOAD send2:");
				exit(-1);
			}

			break;
		}
		else
		{
			buf->cmd = DATA;
			if(0 >= send(sockfd,buf,sizeof(send_data_t),0))
			{
				perror("DOWNLOAD send2:");
				exit(-1);
			}
		}
	}

	fclose(fp);
	return 0;
}



int main(int argc, const char *argv[])
{


	int tcp_socket,acceptfd;
	struct sockaddr_in addr;
	send_data_t buf;

	//socket
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(0 > tcp_socket)
	{
		perror("socket");
		exit(-1);
	}


	addr.sin_family = AF_INET;
	if(argc != 3)
	{
		addr.sin_port = htons(50000);
		addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	}
	else
	{
		addr.sin_port = htons((uint16_t)atoi(argv[2]));
		addr.sin_addr.s_addr = inet_addr(argv[1]);
	}

	//bind
	if(0 > bind(tcp_socket,(SA *)&addr,sizeof(addr)))
	{
		perror("bind");
		exit(-1);
	}
	//listen
	if(0 > listen(tcp_socket,100))
	{
		perror("listen");
		exit(-1);
	}

	while(1)
	{

		acceptfd = accept(tcp_socket,NULL,NULL);
		if(0 > acceptfd)
		{
			perror("accept");
			exit(-1);
		}

		if(0 == fork())
		{
			if(0 == fork())
			{
				close(tcp_socket);//关闭子进程里用不到的监听套接字

				while(1)
				{

					//recv(); //接收客户指令
					if(0 >= recv(acceptfd,&buf,sizeof(buf),0))
					{
						perror("recv");
						exit(0);
					}

					switch(buf.cmd)
					{
					case DOWNLOAD:
						download(acceptfd,&buf);
						break;
					case GET_UP:
						break;
						//	getup();
					}
					//执行指令
					// 1.getup
					// 2.download
					// 3.list
					// 4.quit

				}
			}
			exit(-1);

		}
		close(acceptfd); //关闭爷爷里用不到的 链接套接字
		wait(NULL);
	}

	return 0;
}
