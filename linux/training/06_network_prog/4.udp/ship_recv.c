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

	struct ip_mreqn ship_addr;


	udp_socket = socket(AF_INET, SOCK_DGRAM, 0); //创建一个套接字描述符
	if(0 > udp_socket){
		perror("socket");
		return -0;
	}
/*接受方要加入多播组，设置套接字属性
 */
	ship_addr.imr_address.s_addr = inet_addr("192.168.2.43");//用本地的一个地址 指定一个物理网卡
	ship_addr.imr_ifindex = 0; //不适用接口编号来识别网卡
	ship_addr.imr_multiaddr.s_addr = inet_addr("225.2.3.4");//将要添加的 组播地址

	if(0 > setsockopt(udp_socket,IPPROTO_IP,IP_ADD_MEMBERSHIP,&ship_addr,sizeof(ship_addr)) ){
		perror("IP_ADD_MEMBERSHIP");
	}



	addr.sin_family = AF_INET; //填充套接字地址  man 7 ip
	addr.sin_addr.s_addr = inet_addr("225.2.3.4");//htonl((192 << 24) + (168 << 16) + (0 << 8) + (239 << 0));
	addr.sin_port = htons(50001); //注意网络字节序


	if(0 > bind(udp_socket,(struct sockaddr *)&addr,sizeof(addr))) { //给套接字绑定端口和ip使其地址固定
		perror("bind");
		return -1;
	}
	addrlen = sizeof(src_addr);
	while(1){
		bzero(buf,sizeof(buf));
		if(0 > recvfrom(udp_socket,buf,sizeof(buf),0,
				(SA *) &src_addr,&addrlen) ){
			perror("recvfrom");
			return -1;
		}

		printf("addr:%s port: %d :",inet_ntoa(src_addr.sin_addr),ntohs(src_addr.sin_port));
		puts(buf);
	}

	return 0;
}
