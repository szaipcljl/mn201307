/*
 * netlink.c
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include "usrlink.h"

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("MDAXIA");

struct sock *netlink_fd;


static void netlink_to_user(int dest, void *buf, int len)
{
	struct nlmsghdr *nl;
	struct sk_buff *skb;
	int size;

	size = NLMSG_SPACE(len);
	skb = alloc_skb(size, GFP_ATOMIC);
	if(!skb || !buf)
	{
		printk(KERN_ALERT "netlink_to_user skb of buf null!\n");
		return;
	}
	nl = nlmsg_put(skb, 0, 0, 0, NLMSG_SPACE(len) - sizeof(struct nlmsghdr), 0);
#if 1 //old kernel
	NETLINK_CB(skb).pid = 0;
#else
	NETLINK_CB(skb).nsid = 0;
#endif
	NETLINK_CB(skb).dst_group = 0;

	memcpy(NLMSG_DATA(nl), buf, len);
	nl->nlmsg_len = (len > 2) ? (len - 2):len;

	netlink_unicast(netlink_fd, skb, dest, MSG_DONTWAIT);
	printk(KERN_ALERT "K send packet success\n");
}

static int process_hello_get(int dest, void *buf, int len)
{
	printk(KERN_ALERT "In process_hello get!\n");
	memcpy(buf, "I known you !", 13);
	netlink_to_user(dest, buf, 13);
	return NET_OK;
}

static int process_hello_set(int dest, void *buf, int len)
{
	printk(KERN_ALERT "In process_hello set! %s\n", (char *)buf);
	memcpy(buf, "S known you !", 13);
	netlink_to_user(dest, buf, 13);
	return NET_OK;
}


static void netlink_process_packet(struct nlmsghdr *nl)
{
	int ret;

	switch(nl->nlmsg_type)
	{
	case HELLO_GET:
		ret = process_hello_get(nl->nlmsg_pid, NLMSG_DATA(nl), nl->nlmsg_len);
		break;
	case HELLO_SET:
		ret = process_hello_set(nl->nlmsg_pid, NLMSG_DATA(nl), nl->nlmsg_len);
		break;
	default:break;
	}
}

static void netlink_recv_packet(struct sk_buff *__skb)
{
	struct sk_buff *skb;
	struct nlmsghdr *nlhdr;

	skb = skb_get(__skb);
	if(skb->len >= sizeof(struct nlmsghdr))
	{
		nlhdr = (struct nlmsghdr *)skb->data;
		if(nlhdr->nlmsg_len >= sizeof(struct nlmsghdr) &&
				__skb->len >= nlhdr->nlmsg_len)
		{
			netlink_process_packet(nlhdr);
		}
	}
	else
		printk(KERN_ALERT "Kernel receive msg length error!\n");
}

static int __init netlink_init(void)
{
	netlink_fd = netlink_kernel_create(&init_net, USER_NETLINK_CMD, 0, netlink_recv_packet, NULL, THIS_MODULE);
	if(NULL == netlink_fd)
	{
		printk(KERN_ALERT "Init netlink!\n");
		return -1;
	}
	printk(KERN_ALERT "Init netlink success!\n");
	return 0;
}

static void __exit netlink_exit(void)
{
	netlink_kernel_release(netlink_fd);
	printk(KERN_ALERT "Exit netlink!\n");
}

module_init(netlink_init);
module_exit(netlink_exit);
