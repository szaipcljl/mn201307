#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmr_interface.h"
#include "cmr_dev_info.h"
#include "i2c_io_ops.h"

int i2c_device_open(struct i2c_dev *cmr_dev)
{
	int fd;

	if (!cmr_dev) {
		printf("error: cmr_dev == NULL\n");
		return -1;
	}

	fd = i2c_dev_open(cmr_dev->bus_id, cmr_dev->i2c_addr);
	if (fd < 0) {
		printf("[%s] i2c_dev_open failed\n", __func__);
		return -1;
	}

	return fd;
}

int i2c_device_close(struct i2c_dev *cmr_dev)
{
	if (!cmr_dev) {
		printf("error: cmr_dev == NULL\n");
		return -1;
	}

	if (!cmr_dev->is_open) {
		printf("i2c dev is not opened\n");
		return -1;
	}

	i2c_dev_close(cmr_dev->fd);

	return 0;
}

int set_sensor_temp_on(struct i2c_dev *cmr_dev)
{
	if (!cmr_dev) {
		printf("error: cmr_dev == NULL\n");
		return -1;
	}
	//TODO
	return 0;
}

int get_sensor_temp(struct i2c_dev *cmr_dev)
{
	if (!cmr_dev) {
		printf("error: cmr_dev == NULL\n");
		return -1;
	}
	//TODO
	return 0;
}

//save ir led ctl reg val when ir led on
static int get_sensor_ir_led_ctl_ori_val(struct i2c_dev *cmr_dev)
{
	int ret;
	if (!cmr_dev) {
		printf("error: cmr_dev == NULL\n");
		return -1;
	}

	if (cmr_dev->ir_led_ctl_ori_val_saved) {
		return 0;
	}

	ret = read_16bit_reg(cmr_dev->fd, cmr_dev->ir_led_ctl_reg, \
			(unsigned int *)&cmr_dev->ir_led_ctl_ori_val);
	if (ret < 0) {
		printf("[%s] read_16bit_reg read reg[%x] failed\n",\
				__func__, cmr_dev->ir_led_ctl_reg);
		return -1;
	}

	if (cmr_dev->ir_led_ctl_ori_val & CMR_FLASH_ON_MASk) {
		printf("ir led ctl origin value saved\n");
		cmr_dev->ir_led_ctl_ori_val_saved = 1;
	} else {
		printf("[%s]you should get ir led ctl reg ori value when led is on\n", __func__);
		return -1;
	}

	return 0;
}

int get_sensor_ir_led_status(struct i2c_dev *cmr_dev)
{
	int ret;

	if (!cmr_dev) {
		printf("error: cmr_dev == NULL\n");
		return -1;
	}

	get_sensor_ir_led_ctl_ori_val(cmr_dev);

	ret = read_16bit_reg(cmr_dev->fd, cmr_dev->ir_led_ctl_reg, \
			(unsigned int *)&cmr_dev->ir_led_ctl_val);
	if (ret < 0) {
		printf("[%s] read_16bit_reg read reg[%x] failed\n",\
				__func__, cmr_dev->ir_led_ctl_reg);
		return -1;
	}

	cmr_dev->ir_led_status = cmr_ir_led_status(cmr_dev->ir_led_ctl_val);

	return cmr_dev->ir_led_status;
}


int set_sensor_ir_led_on(struct i2c_dev *cmr_dev)
{
	int ret;
	if (!cmr_dev) {
		printf("error: cmr_dev == NULL\n");
		return -1;
	}

	get_sensor_ir_led_status(cmr_dev);

	if (cmr_dev->ir_led_status) {
		printf("[%s] ir led is already on\n", __func__);
		return 0;
	}

	ret = write_16bit_reg(cmr_dev->fd, cmr_dev->ir_led_ctl_reg, (unsigned int)(cmr_dev->ir_led_ctl_ori_val | CMR_FLASH_ON_MASk));
	if (ret < 0) {
		printf("[%s] write_16bit_reg write reg[%x] failed\n",\
				__func__, cmr_dev->ir_led_ctl_reg);
		return -1;
	}

	cmr_dev->ir_led_status = IR_LED_ON;
	printf("[%s] write reg to set ir led on\n", __func__);

	return 0;
}

int set_sensor_ir_led_off(struct i2c_dev *cmr_dev)
{
	int ret;

	if (!cmr_dev) {
		printf("error: cmr_dev == NULL\n");
		return -1;
	}

	get_sensor_ir_led_status(cmr_dev);

	if (!cmr_dev->ir_led_status) {
		printf("[%s] ir led is already off\n", __func__);
		return 0;
	}

	ret = write_16bit_reg(cmr_dev->fd, cmr_dev->ir_led_ctl_reg, (unsigned int)(cmr_dev->ir_led_ctl_ori_val & (~CMR_FLASH_ON_MASk)));
	if (ret < 0) {
		printf("[%s] write_16bit_reg write reg[%x] failed\n",\
				__func__, cmr_dev->ir_led_ctl_reg);
		return -1;
	}

	printf("[%s] write reg to set ir led off\n", __func__);
	cmr_dev->ir_led_status = IR_LED_OFF;

	return 0;
}

//
static struct cmr_usr_ops cmr_ops = {
	.set_sensor_temp_on = set_sensor_temp_on,
	.get_sensor_temp = get_sensor_temp,
	.get_sensor_ir_led_status = get_sensor_ir_led_status,
	.set_sensor_ir_led_on = set_sensor_ir_led_on,
	.set_sensor_ir_led_off = set_sensor_ir_led_off,
};

// int i2c_dev struct and open the dev
struct i2c_dev *i2c_dev_init()
{

	struct i2c_dev *pcmr_dev;

	pcmr_dev = malloc(sizeof(struct i2c_dev));
	if (!pcmr_dev) {
		printf("malloc struct i2c_dev object failed \n");
		return NULL;
	}

	memset(pcmr_dev, 0, sizeof(struct i2c_dev));

	pcmr_dev->bus_id = CMR_BUS_ID;
	pcmr_dev->i2c_addr = CMR_DEV_I2C_ADDR;
	pcmr_dev->ops = &cmr_ops;
	pcmr_dev->ir_led_ctl_reg = CMR_FLASH_CTL;
	pcmr_dev->temp_ctl_reg = CMR_TEMP_CTL;
	pcmr_dev->temp_reg = CMR_TEMP_REG;
	pcmr_dev->ir_led_status = 1; //default

	pcmr_dev->fd = i2c_device_open(pcmr_dev);
	if (pcmr_dev->fd < 0) {
		printf("[%s] i2c_device_open failed\n", __func__);
		goto failed;
	}

	pcmr_dev->is_open = 1;

	return pcmr_dev;

failed:
	free(pcmr_dev);
	return NULL;
}

// free i2c_dev struct and close i2c dev
int i2c_dev_release(struct i2c_dev *cmr_dev)
{
	if (!cmr_dev) {
		printf("error: cmr_dev == NULL\n");
		return -1;
	}

	i2c_device_close(cmr_dev);

	free(cmr_dev);

	return 0;
}
