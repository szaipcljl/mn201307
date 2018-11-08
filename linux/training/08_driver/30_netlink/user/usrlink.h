/*
 * usrlink.h
 *
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <asm/types.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#ifndef USRLINK_H_
#define USRLINK_H_

#define USER_NETLINK_CMD	25
#define MAXMSGLEN 			1024

typedef enum error_e {
	NET_ERROR,
	NET_OK,
	NET_PARAM,
	NET_MEM,
	NET_SOCK,
} netlink_err;

typedef enum module_e {
	HELLO_CMD = 1,
} netlink_module;

typedef enum type_e {
	HELLO_SET,
	HELLO_GET,
} netlink_type;

typedef struct usr_sock_h {
	int sock;
	struct sockaddr_nl dest;
	struct sockaddr_nl src;
} netlink_sock;

int netlink_sock_init(netlink_sock *netlink_s, int module, int protocol);
int netlink_sock_deinit(netlink_sock *netlink_s);
int netlink_send(netlink_sock *netlink_s, int type, char *sbuf, int slen, char *rbuf, int *rlen);

#endif /* USRLINK_H_ */
