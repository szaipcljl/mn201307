#include <stdio.h>
#include <sys/types.h>	       /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	int udp_socket;
	struct sockaddr_in addr,dest_addr;


	udp_socket = socket(AF_INET, SOCK_DGRAM, 0); //创建一个套接字描述符
#if 0
	addr.sin_family = AF_INET; //填充套接字地址  man 7 ip
	addr.sin_addr.s_addr = inet_addr("192.168.0.239");//htonl((192 << 24) + (168 << 16) + (0 << 8) + (239 << 0));
	addr.sin_port = htons(50000); //注意网络字节序

	bind(udp_socket,(struct sockaddr *)&addr,sizeof(addr)); //给套接字绑定端口和ip使其地址固定
#endif
	dest_addr.sin_family = AF_INET;  //填充目的地址结构体
	dest_addr.sin_addr.s_addr = inet_addr("192.168.0.239");
	dest_addr.sin_port = htons(50001);

	while(1){//向 接收端循环发送 数据
		sendto(udp_socket,"wellcome to farsight !!\n",strlen("wellcome to farsight !!\n") + 1,0
				,(struct sockaddr *)&dest_addr,sizeof(dest_addr));

		sleep(1); //注意 不要发送的太快，死机自负
	}
	return 0;
}
