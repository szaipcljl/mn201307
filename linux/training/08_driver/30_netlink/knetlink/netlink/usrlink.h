/*
 * usrlink.h
 *
 */
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

#endif /* USRLINK_H_ */
