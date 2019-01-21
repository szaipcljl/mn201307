#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define SA struct sockaddr


int main(int argc, const char *argv[])
{
	int tcp_socket;
	struct sockaddr_in addr,dest_addr;


	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(0 > tcp_socket){
		perror("socket");
		exit(-1);
	}

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(50000);
	dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(0 > connect(tcp_socket,(SA *)&dest_addr,sizeof(dest_addr)) ){
		perror("connect");
		return -1;
	}

	while(1){
		if(0 >= send(tcp_socket,"welcome to farsight!!\n",23,0)){
			perror("send");
			return -1;
		}
		sleep(1);
	}



	return 0;
}
