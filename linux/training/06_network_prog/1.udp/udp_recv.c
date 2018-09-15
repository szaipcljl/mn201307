#include <stdio.h>
#include <sys/types.h>	       /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define SA struct sockaddr
int main(int argc, const char *argv[])
{
	int udp_socket;
	struct sockaddr_in addr,src_addr;
	int addrlen;
	char buf[128];


	udp_socket = socket(AF_INET, SOCK_DGRAM, 0); //创建一个套接字描述符

	addr.sin_family = AF_INET; //填充套接字地址  man 7 ip
	addr.sin_addr.s_addr = inet_addr("192.168.0.239");//htonl((192 << 24) + (168 << 16) + (0 << 8) + (239 << 0));
	addr.sin_port = htons(50001); //注意网络字节序


	bind(udp_socket,(struct sockaddr *)&addr,sizeof(addr)); //给套接字绑定端口和ip使其地址固定

	addrlen = sizeof(src_addr);
	while(1){
		bzero(buf,sizeof(buf));
		recvfrom(udp_socket,buf,sizeof(buf),0,
				(SA *) &src_addr,&addrlen);

		printf("addr:%s port: %d :",inet_ntoa(src_addr.sin_addr),ntohs(src_addr.sin_port));
		puts(buf);
	}

	return 0;
}
