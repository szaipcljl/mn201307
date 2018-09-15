#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define SA struct sockaddr
#include "type.h"


int main(int argc, const char *argv[])
{
	int tcp_socket;
	struct sockaddr_in addr,dest_addr;
	FILE *fp;
	char buf[32];
	int datalen;


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

	fp = fopen("a.c","r");



	while(1){
		datalen = fread(buf,1,sizeof(buf),fp);
		if(datalen <= 0){
			close(tcp_socket);
			fclose(fp);
			exit(0);
		}

		if(0 >= send(tcp_socket,buf,datalen,0)){
			perror("send");
			return -1;
		}

	}



	return 0;
}
