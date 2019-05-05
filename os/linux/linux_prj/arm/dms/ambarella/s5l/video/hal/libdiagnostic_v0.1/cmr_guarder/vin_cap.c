#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <sched.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <signal.h>
#include <basetypes.h>

#include "fault_detector/cmr_fault_detector.h"
#include "iav_ioctl.h"
#include "amba_netlink.h"

#include "../../../../device/s5l/unit_test/private/vin_test/vin_init.c"

#include "diagnostic.h"

int signal_write_fd;
static void notify_vsync_lost(void)
{
	if (TEMP_FAILURE_RETRY(write(signal_write_fd, VSYNC_LOST, 1)) == -1) {
		printf("write(signal_write_fd)[vsync_lost] failed: %s\n", strerror(errno));
	}
}

extern struct cmr_fault_detector cmr_ft_dtect;

struct nl_vsync_config {
	s32 fd_nl;
	s32 nl_connected;
	struct amba_nl_msg_data msg;
	char nl_send_buf[AMBA_NL_MAX_MSG_LEN];
	char nl_recv_buf[AMBA_NL_MAX_MSG_LEN];
};

static int recover_vin_cap(u16 vinc_map);

int fd_iav;
static struct nl_vsync_config vsync_config;


static int init_netlink()
{
	u32 pid;
	struct sockaddr_nl saddr;

	vsync_config.fd_nl = socket(AF_NETLINK, SOCK_RAW, AMBA_NL_PORT_ERROR);
	memset(&saddr, 0, sizeof(saddr));
	pid = getpid();
	saddr.nl_family = AF_NETLINK;
	saddr.nl_pid = pid;
	saddr.nl_groups = 1;
	saddr.nl_pad = 0;
	bind(vsync_config.fd_nl, (struct sockaddr *)&saddr, sizeof(saddr));

	vsync_config.nl_connected = 0;

	return 0;
}

static int send_vsync_msg_to_kernel(struct amba_nl_msg_data vsync_msg)
{
	struct sockaddr_nl daddr;
	struct msghdr msg;
	struct nlmsghdr *nlhdr = NULL;
	struct iovec iov;

	memset(&daddr, 0, sizeof(daddr));
	daddr.nl_family = AF_NETLINK;
	daddr.nl_pid = 0;
	daddr.nl_groups = 1;
	daddr.nl_pad = 0;

	nlhdr = (struct nlmsghdr *)vsync_config.nl_send_buf;
	nlhdr->nlmsg_pid = getpid();
	nlhdr->nlmsg_len = NLMSG_LENGTH(sizeof(vsync_msg));
	nlhdr->nlmsg_flags = 0;
	memcpy(NLMSG_DATA(nlhdr), &vsync_msg, sizeof(vsync_msg));

	memset(&msg, 0, sizeof(struct msghdr));
	iov.iov_base = (void *)nlhdr;
	iov.iov_len = nlhdr->nlmsg_len;
	msg.msg_name = (void *)&daddr;
	msg.msg_namelen = sizeof(daddr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	sendmsg(vsync_config.fd_nl, &msg, 0);

	return 0;
}

static int recv_vsync_msg_from_kernel()
{
	struct sockaddr_nl sa;
	struct nlmsghdr *nlhdr = NULL;
	struct msghdr msg;
	struct iovec iov;

	int ret = 0;

	nlhdr = (struct nlmsghdr *)vsync_config.nl_recv_buf;
	iov.iov_base = (void *)nlhdr;
	iov.iov_len = AMBA_NL_MAX_MSG_LEN;

	memset(&sa, 0, sizeof(sa));
	memset(&msg, 0, sizeof(msg));
	msg.msg_name = (void *)&(sa);
	msg.msg_namelen = sizeof(sa);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	if (vsync_config.fd_nl > 0) {
		ret = recvmsg(vsync_config.fd_nl, &msg, 0);
	} else {
		printf("Netlink socket is not opened to receive message!\n");
		ret = -1;
	}

	return ret;
}

static int check_recv_vsync_msg()
{
	struct nlmsghdr *nlhdr = NULL;
	int msg_len;

	nlhdr = (struct nlmsghdr *)vsync_config.nl_recv_buf;
	if (nlhdr->nlmsg_len <  sizeof(struct nlmsghdr)) {
		printf("Corruptted kernel message!\n");
		return -1;
	}
	msg_len = nlhdr->nlmsg_len - NLMSG_LENGTH(0);
	if (msg_len < sizeof(struct amba_nl_msg_data)) {
		printf("Unknown kernel message!!\n");
		return -1;
	}

	return 0;
}

static int process_vsync_req(struct amba_nl_msg_data *nl_msg)
{
	int ret = 0;
	struct amba_nl_msg_vsync_data *data = NULL;

	if (nl_msg->msg == AMBA_NL_MSG_ERR_IAV_VSYNC_LOST) {
		data = (struct amba_nl_msg_vsync_data *)(nl_msg->payload);

		//cmr_ft_record_work(&cmr_ft_dtect);
		//TODO
		notify_vsync_lost();

		ret = recover_vin_cap(data->vinc_map);
		vsync_config.msg.src_pid = getpid();
		vsync_config.msg.dst_pid = 0;
		vsync_config.msg.port = AMBA_NL_PORT_ERROR;
		vsync_config.msg.type = AMBA_NL_MSG_TYPE_NORMAL;
		vsync_config.msg.dir = AMBA_NL_MSG_DIR_STATUS;
		vsync_config.msg.msg = AMBA_NL_MSG_ERR_IAV_VSYNC_LOST;
		if (ret < 0) {
			vsync_config.msg.status = AMBA_NL_MSG_STATUS_FAIL;
			send_vsync_msg_to_kernel(vsync_config.msg);
		} else {
			vsync_config.msg.status = AMBA_NL_MSG_STATUS_SUCCESS;
			send_vsync_msg_to_kernel(vsync_config.msg);
		}
	} else {
		printf("Unrecognized kernel message!\n");
		ret = -1;
	}

	return ret;
}

static int process_vsync_session_status(struct amba_nl_msg_data *kernel_msg)
{
	int ret = 0;

	if (kernel_msg->type != AMBA_NL_MSG_TYPE_SESSION ||
		kernel_msg->dir != AMBA_NL_MSG_DIR_STATUS) {
		return -1;
	}

	switch (kernel_msg->msg) {
	case AMBA_NL_MSG_SESS_CONNECT:
		if (kernel_msg->status == AMBA_NL_MSG_STATUS_SUCCESS) {
			vsync_config.nl_connected = 1;
			printf("Connection established with kernel.\n");
		} else {
			vsync_config.nl_connected = 0;
			printf("Failed to establish connection with kernel!\n");
		}
		break;
	case AMBA_NL_MSG_SESS_DISCONNECT:
		vsync_config.nl_connected = 0;
		if (kernel_msg->status == AMBA_NL_MSG_STATUS_SUCCESS) {
			printf("Connection removed with kernel.\n");
		} else {
			printf("Failed to remove connection with kernel!\n");
		}
		break;
	default:
		printf("Unrecognized session cmd from kernel!\n");
		ret = -1;
		break;
	}

	return ret;
}

static int process_vsync_msg()
{
	struct nlmsghdr *nlhdr = NULL;
	struct amba_nl_msg_data *kernel_msg;
	int ret = 0;

	if (check_recv_vsync_msg() < 0) {
		return -1;
	}

	nlhdr = (struct nlmsghdr *)vsync_config.nl_recv_buf;
	kernel_msg = (struct amba_nl_msg_data *)NLMSG_DATA(nlhdr);

	if(kernel_msg->type == AMBA_NL_MSG_TYPE_NORMAL&&
		kernel_msg->dir == AMBA_NL_MSG_DIR_MSG) {
		if (process_vsync_req(kernel_msg) < 0) {
			ret = -1;
		}
	} else if (kernel_msg->type == AMBA_NL_MSG_TYPE_SESSION &&
				kernel_msg->dir == AMBA_NL_MSG_DIR_STATUS) {
		if (process_vsync_session_status(kernel_msg) < 0) {
			ret = -1;
		}
	} else {
		printf("Incorrect message from kernel!\n");
		ret = -1;
	}

	return ret;
}

static int nl_send_vsync_session_cmd(int cmd)
{
	int ret = 0;

	vsync_config.msg.src_pid = getpid();
	vsync_config.msg.dst_pid = 0;
	vsync_config.msg.port = AMBA_NL_PORT_ERROR;
	vsync_config.msg.type = AMBA_NL_MSG_TYPE_SESSION;
	vsync_config.msg.dir = AMBA_NL_MSG_DIR_MSG;
	vsync_config.msg.msg = cmd;
	vsync_config.msg.status = 0xFF;
	send_vsync_msg_to_kernel(vsync_config.msg);

	ret = recv_vsync_msg_from_kernel();

	if (ret > 0) {
		ret = process_vsync_msg();
		if (ret < 0) {
			printf("Failed to process session status!\n");
		}
	} else {
		printf("Error for getting session status!\n");
	}

	return ret;
}

static void * netlink_loop(void * data)
{
	int ret;
	int count = 100;

	while (count && !vsync_config.nl_connected) {
		if (nl_send_vsync_session_cmd(AMBA_NL_MSG_SESS_CONNECT) < 0) {
			printf("Failed to establish connection with kernel!\n");
		}
		sleep(1);
		count--;
	}

	if (!vsync_config.nl_connected) {
		printf("Please enable kernel vsync loss guard mechanism!!!\n");
		return NULL;
	}

	while (vsync_config.nl_connected) {
		ret = recv_vsync_msg_from_kernel();
		if (ret > 0) {
			ret = process_vsync_msg();
			if (ret < 0) {
				printf("Failed to process the msg from kernel!\n");
			}
		}
		else {
			printf("Error for getting msg from kernel!\n");
		}
	}

	return NULL;
}

static u32 get_vsrc_num()
{
	struct vindev_devinfo vin_info;

	vin_info.vsrc_id = 0;
	if (ioctl(fd_iav, IAV_IOC_VIN_GET_DEVINFO, &vin_info) < 0) {
		perror("IAV_IOC_VIN_GET_DEVINFO error\n");
		return -1;
	}

	return vin_info.vsrc_num;
}

static u32 get_vsrc_vin_id(u32 vsrc_id)
{
	struct vindev_devinfo vin_info;

	vin_info.vsrc_id = vsrc_id;
	if (ioctl(fd_iav, IAV_IOC_VIN_GET_DEVINFO, &vin_info) < 0) {
		perror("IAV_IOC_VIN_GET_DEVINFO error\n");
		return -1;
	}

	return vin_info.vinc_id;
}

static int vin_cap_resume()
{
	struct iav_vcap_cfg cfg;
	int ret = 0;

	cfg.cid = IAV_VCAP_CFG_STATE;
	cfg.arg.state = IAV_VCAP_STATE_ACTIVE;
	ret = ioctl(fd_iav, IAV_IOC_SET_VCAP_CFG, &cfg);

	return ret;
}

static int reset_input_vin(int vin_id)
{
	struct vindev_mode video_info;
	struct vindev_fps vsrc_fps;
	u32 vsrc_id;
	u32 vsrc_num = get_vsrc_num();

	// select channel: for multi channel VIN (initialize)
	if (channel >= 0) {
		if (select_channel() < 0)
			return -1;
	}

	for (vsrc_id = 0; vsrc_id < vsrc_num; vsrc_id++) {
		if (get_vsrc_vin_id(vsrc_id) == vin_id) {
			memset(&video_info, 0, sizeof(video_info));
			video_info.vsrc_id = vsrc_id;
			if(ioctl(fd_iav, IAV_IOC_VIN_GET_MODE, &video_info) < 0) {
				return -1;
			} else {
				printf("Start to restore vsrc[%d] vin_mode 0x%x and hdr_mode %d.\n",
					vsrc_id, video_info.video_mode, video_info.hdr_mode);
			}

			vsrc_fps.vsrc_id = vsrc_id;
			if(ioctl(fd_iav, IAV_IOC_VIN_GET_FPS, &vsrc_fps) < 0) {
				return -1;
			} else {
				printf("Start to restore vin frame rate %d.\n", vsrc_fps.fps);
			}

			if(ioctl(fd_iav, IAV_IOC_VIN_SET_MODE, &video_info) < 0) {
				return -1;
			} else {
				printf("Succeed to restore vsrc[%d] vin_mode 0x%x and hdr_mode %d.\n",
					vsrc_id, video_info.video_mode, video_info.hdr_mode);
			}

			if (ioctl(fd_iav, IAV_IOC_VIN_SET_FPS, &vsrc_fps) < 0) {
				perror("IAV_IOC_VIN_SET_FPS");
				return -1;
			} else {
				printf("Succeed to restore vin frame rate %d.\n", vsrc_fps.fps);
			}

			break; // for B5/B6 cases, only need to config one vsrc
		}
	}
	return 0;
}

static int reset_input(int vin_id)
{
	int vinc_id, ret = 0;

	if (vin_id >= VIN_CONTROLLER_NUM) {
		for (vinc_id = 0; vinc_id < VIN_CONTROLLER_NUM; vinc_id++) {
			ret = reset_input_vin(vinc_id);
			if (ret < 0) {
				break;
			}
		}
	} else {
		ret = reset_input_vin(vin_id);
	}

	return ret;
}

static int recover_vin_cap(u16 vinc_map)
{
	int ret = 0, i;

	for (i = 0; i < VIN_CONTROLLER_NUM; i++) {
		if (vinc_map & (1 << i)) {
			ret = reset_input(i);
			if (ret) {
				return ret;
			}
		}
	}
	ret = vin_cap_resume();
	if (ret) {
		return ret;
	}

	return ret;
}

void *vin_cap_auto(void *arg)
{
	signal_write_fd = *(int *)arg;

	init_netlink();
	netlink_loop(NULL);

	return NULL;
}
