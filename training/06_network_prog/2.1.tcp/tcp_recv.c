#include <stdio.h>
#include <sys/types.h>	       /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>

#define SA struct sockaddr


int main(int argc, const char *argv[])
{
	int tcp_socket,acceptfd;
	struct sockaddr_in addr,src_addr;
	int addrlen;
	char buf[128];

	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(0 > tcp_socket){
		perror("socket");
		exit(-1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(50000);
	addr.sin_addr.s_addr = inet_addr("0.0.0.0");

	if(0 > bind(tcp_socket,(SA *)&addr,sizeof(addr))){
		perror("bind");
		exit(-1);
	}



	if(0 >listen(tcp_socket,10) ){
		perror("listen");
		exit(-1);
	}

	addrlen = sizeof(src_addr);
	if( 0 > (acceptfd = accept(tcp_socket,(SA *)&src_addr ,&addrlen) ) ){
		perror("listen");
		exit(-1);
	}

	while(1){
		bzero(buf,sizeof(buf));
		if(0 >= recv(acceptfd,buf,sizeof(buf),0)){
			perror("recv");
			exit(-1);
		}

		puts(buf);


	}


	return 0;
}
