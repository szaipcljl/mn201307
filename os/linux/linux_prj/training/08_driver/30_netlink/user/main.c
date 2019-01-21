/*
 * main.c
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usrlink.h"

int parse_ret(int ret)
{
	switch(ret)
	{
	case NET_OK:
		return ret;
	case NET_ERROR:
		printf("error\n");
		goto exit_p;
	case NET_MEM:
		printf("Memory error\n");
		goto exit_p;
	case NET_PARAM:
		printf("Param error\n");
		goto exit_p;
	case NET_SOCK:
		printf("Socket error\n");
		goto exit_p;
	default:break;
	}
exit_p:
	return NET_ERROR;
}

void usage(void)
{
	printf("Usage: Netlink -G <param>\n\t-S <param>\n");
}

int main(int argc, char **argv)
{
	netlink_sock h_sock;
	char rbuf[1024];
	char sbuf[1024];
	int ret, type, slen = 0, rlen = 0;

	ret = netlink_sock_init(&h_sock, HELLO_CMD, USER_NETLINK_CMD);
	if(NET_OK != parse_ret(ret))
		goto exit_p;

	bzero(&rbuf, sizeof(rbuf));
	bzero(&sbuf, sizeof(sbuf));
	if(argc < 3)
	{
		usage();
		goto exit_p;
	}
	if(!strncmp("-G", argv[1], 2))
		type = HELLO_GET;
	else if(!strncmp("-S", argv[1], 2))
		type = HELLO_SET;

	strcpy(sbuf, argv[2]);
	slen = strlen(sbuf);
	ret = netlink_send(&h_sock, type, sbuf, slen, rbuf, &rlen);
	if(NET_OK != parse_ret(ret))
		goto exit_p;

	if(rlen > 0)
	{
		rbuf[rlen] = '\0';
		printf("K rep [len = %d]:%s\n", rlen, rbuf);
	}
	printf("K[len = %d]: %s\n", rlen, rbuf);

exit_p:
	netlink_sock_deinit(&h_sock);
	return 0;
}
