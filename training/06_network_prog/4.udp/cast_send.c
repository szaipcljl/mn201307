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
	int flag;

	udp_socket = socket(AF_INET, SOCK_DGRAM, 0); //创建一个套接字描述符
	if(0 > udp_socket){
		perror("socket");
		return -1;
	}

	flag = 1;
	setsockopt(udp_socket,SOL_SOCKET,SO_BROADCAST,&flag,sizeof(flag));

	dest_addr.sin_family = AF_INET;  //填充目的地址结构体
	dest_addr.sin_addr.s_addr = inet_addr("192.168.0.255");
	dest_addr.sin_port = htons(50001);

	while(1){//向 接收端循环发送 数据
		if(0 >= sendto(udp_socket,"wellcome to farsight !!\n",strlen("wellcome to farsight !!\n") + 1,0
				,(struct sockaddr *)&dest_addr,sizeof(dest_addr)) ){
			perror("send");
			return -1;
		}
		sleep(1); //注意 不要发送的太快，死机自负
	}
	return 0;
}
