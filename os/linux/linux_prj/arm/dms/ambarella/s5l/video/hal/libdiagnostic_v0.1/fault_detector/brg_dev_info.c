#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fault_detector/brg_dev_info.h"
#include "utils/i2c_io_ops.h"

int i2c_device_open(struct i2c_dev_base_info *dev)
{
	int fd;

	if (!dev) {
		printf("error: dev == NULL\n");
		return -1;
	}

	fd = i2c_dev_open(dev->bus_id, dev->i2c_addr);
	if (fd < 0) {
		printf("[%s] i2c_dev_open failed\n", __func__);
		return -1;
	}

	return fd;
}

int i2c_device_close(struct i2c_dev_base_info *dev)
{
	if (!dev) {
		printf("error: dev == NULL\n");
		return -1;
	}

	if (!dev->is_open) {
		printf("i2c dev is not opened\n");
		return -1;
	}

	i2c_dev_close(dev->fd);

	return 0;
}

static int serdes_get_device_status(struct brg_dev_info *brg_info)
{
	int ret;

	if (!brg_info) {
		printf("error: brg_info == NULL\n");
		return -1;
	}

	ret = read_8bit_reg(brg_info->i2c_info.fd, brg_info->brg_regs[BRG_STS], \
			(unsigned char *)&brg_info->status);
	if (ret < 0) {
		printf("[%s] read_8bit_reg read reg[%x] failed\n",\
				__func__, brg_info->brg_regs[BRG_STS]);
		return -1;
	}
	printf("[%s] brg status: 0x%x\n", __func__, brg_info->status);

	return 0;
}

static int	brg_get_lock_pin_status(struct brg_dev_info *brg_info)
{
	int ret;
	unsigned char sts;

	ret = serdes_get_device_status(brg_info);
	if (ret < 0) {
		printf("[%s] serdes_get_device_status read reg[%x] failed\n",\
				__func__, brg_info->status);
		return -1;
	}

	sts = brg_info->status;

	return !!(sts & (1<<2));
}

static int brg_get_pass_pin_status(struct brg_dev_info *brg_info)
{
	int ret;
	unsigned char sts;

	ret = serdes_get_device_status(brg_info);
	if (ret < 0) {
		printf("[%s] serdes_get_device_status read reg[%x] failed\n",\
				__func__, brg_info->status);
		return -1;
	}

	sts = brg_info->status;

	return !!(sts & (1<<3));
}

static struct brg_usr_ops brg_ops = {
	.brg_get_lock_pin_status = brg_get_lock_pin_status,
	.brg_get_pass_pin_status = brg_get_pass_pin_status,
};

// int brg device struct and open the brg device
struct brg_dev_info *brg_device_init()
{

	struct brg_dev_info *pdev;

	pdev = malloc(sizeof(struct brg_dev_info));
	if (!pdev) {
		printf("malloc struct brg_dev_info object failed \n");
		return NULL;
	}

	memset(pdev, 0, sizeof(struct brg_dev_info));

	pdev->i2c_info.bus_id = BRG_BUS_ID;
	pdev->i2c_info.i2c_addr = BRG_I2C_ADDR;
	pdev->brg_regs[BRG_STS] = DEVICE_STS;

	pdev->ops = &brg_ops;

	pdev->i2c_info.fd = i2c_device_open(&pdev->i2c_info);
	if (pdev->i2c_info.fd < 0) {
		printf("[%s] i2c_device_open failed\n", __func__);
		goto failed;
	}

	pdev->i2c_info.is_open = 1;

	return pdev;

failed:
	free(pdev);
	return NULL;
}

// free brg device struct and close brg device
int brg_device_release(struct brg_dev_info *dev)
{
	if (!dev) {
		printf("error: dev == NULL\n");
		return -1;
	}

	i2c_device_close(&dev->i2c_info);

	free(dev);

	return 0;
}
