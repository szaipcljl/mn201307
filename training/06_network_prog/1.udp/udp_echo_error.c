#include <stdio.h>
#include <sys/types.h>	       /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define SA struct sockaddr
int main(int argc, const char *argv[])
{
	int udp_socket,udp_socket2;
	struct sockaddr_in addr,src_addr;
	int addrlen;
	char buf[128];
	int len;

	udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(0 > udp_socket)
		perror("socket");

	udp_socket2 = socket(AF_INET, SOCK_DGRAM, 0);
	if(0 > udp_socket2)
		perror("socket");

	addr.sin_family = AF_INET; //填充套接字地址  man 7 ip
	addr.sin_addr.s_addr = inet_addr("0.0.0.0");//htonl((192 << 24) + (168 << 16) + (0 << 8) + (239 << 0));
	addr.sin_port = htons(50001); //注意网络字节序


	if( 0 > bind(udp_socket2,(struct sockaddr *)&addr,sizeof(addr)) ){
			//给套接字绑定端口和ip使其地址固定 )
			perror("bind");
	}
	if( 0 > bind(udp_socket,(struct sockaddr *)&addr,sizeof(addr)) ){ //端口被 udp_socket2 占用
			//给套接字绑定端口和ip使其地址固定 )
			perror("bind");
	}


	addrlen = sizeof(src_addr);
	while(1){
		bzero(buf,sizeof(buf)); //因为端口绑定失败，所以当第一次传输数据时，系统随机分配端口
		if(0 >= (len = recvfrom(udp_socket,buf,sizeof(buf),0,
				(SA *) &src_addr,&addrlen) ) ){
			perror("recvfrom");
		}

		printf("addr:%s port: %d :",inet_ntoa(src_addr.sin_addr),ntohs(src_addr.sin_port));
		puts(buf);

		if(0 >= sendto(udp_socket,buf,len,0,(SA *)&src_addr,addrlen) ){
			perror("sendto");
		}

	}

	return 0;
}
