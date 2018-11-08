/*
 * usrlink.c
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "usrlink.h"

int netlink_sock_init(netlink_sock *netlink_s, int module, int protocol)
{
	netlink_s->sock = socket(PF_NETLINK, SOCK_RAW, protocol);
	if(netlink_s->sock < 0)
		return NET_SOCK;
	memset(&netlink_s->src, 0 ,sizeof(netlink_s->src));
	netlink_s->src.nl_family = AF_NETLINK;
	netlink_s->src.nl_pid = module;
	netlink_s->src.nl_groups = 0;

	if(bind(netlink_s->sock, (struct sockaddr *)&netlink_s->src, sizeof(netlink_s->src)) < 0)
		return NET_SOCK;

	netlink_s->dest.nl_family = AF_NETLINK;
	netlink_s->dest.nl_pid = 0;
	netlink_s->dest.nl_groups = 0;

	return NET_OK;
}

int netlink_send(netlink_sock *netlink_s, int type, char *sbuf, int slen, char *rbuf, int *rlen)
{
	struct msghdr msg;
	struct nlmsghdr *nlhdr = NULL;
	struct iovec iov;
	int ret;

	nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAXMSGLEN));
	if(NULL == nlhdr)
		return NET_MEM;

	memcpy(NLMSG_DATA(nlhdr), sbuf, slen);
	nlhdr->nlmsg_len = NLMSG_SPACE(slen);
	nlhdr->nlmsg_pid = netlink_s->src.nl_pid;
	nlhdr->nlmsg_type = type;
	nlhdr->nlmsg_flags = 0;

	iov.iov_base = (void *)nlhdr;
	iov.iov_len = nlhdr->nlmsg_len;

	msg.msg_name = (void *)&(netlink_s->dest);
	msg.msg_namelen = sizeof(netlink_s->dest);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	ret = sendmsg(netlink_s->sock, &msg, 0);
	if(ret < 0)
	{
		printf("Send fail\n");
		goto error;
	}
	ret = recvmsg(netlink_s->sock, &msg, 0);
	if(ret < 0)
	{
		printf("Read fail\n");
		goto error;
	}
	memcpy(rbuf, NLMSG_DATA(nlhdr), nlhdr->nlmsg_len);
	*rlen = nlhdr->nlmsg_len;
	return NET_OK;

error:
	free(nlhdr);
	return NET_SOCK;
}

int netlink_sock_deinit(netlink_sock *netlink_s)
{
	close(netlink_s->sock);
	memset(netlink_s, 0, sizeof(netlink_sock));
	return NET_OK;
}
