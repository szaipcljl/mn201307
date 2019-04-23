#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>

#include "i2c_io_ops.h"

int i2c_dev_open(int bus_id, int i2c_addr)
{
	int fd;
	int status;

	// OPENING I2C DEVICE
	if (bus_id == 0) {
		fd = open(I2C_BUS_DEV0, O_RDWR);
	} else if (bus_id == 1) {
		fd = open(I2C_BUS_DEV1, O_RDWR);
	} else {
		//fd = open(I2C_BUS_DEV2, O_RDWR);
		//todo
		fd = -1;
	}

	if (fd < 0) {
		printf("[%s] ERROR: open %s(%d) failed\n", I2C_BUS_DEV, __func__, fd);
		close(fd);
		return -1;
	}

	ioctl(fd, I2C_TENBIT, 0);
	// SETTING i2c slave ADDR

	status = ioctl(fd, I2C_SLAVE, i2c_addr);
	if (status < 0)  {
		printf("[%s] ERROR: ioctl(fd, I2C_SLAVE, 0x%02X) failed\n", __func__, i2c_addr);
		close(fd);
		return -1;
	}

	return fd;
}

int i2c_dev_close(int fd)
{
	close(fd);
	return 0;
}

int read_8bit_reg(int fd, unsigned char* reg, unsigned char* rbuf)
{
	//set reg addr to read
	if(write(fd, reg, 1)!=1){
		printf("[%s] ERROR: write reg[0x%02x] failed\n", __func__, *reg);
		return -1;
	}

	if (read(fd, rbuf,1) != 1) {
		printf("[%s] ERROR: read reg[0x%02x] failed\n", __func__, *reg);
		return -1;
	}

	return 0;
}

int write_8bit_reg(int fd, unsigned char* reg, unsigned char* val)
{
	unsigned char wbuf[2];

	wbuf[0] = *reg & 0xff; //8bit reg
	wbuf[1] = *val;

	//write 8bit reg; first byte is reg addr
	if (write(fd, wbuf, 2) != 2){
		printf("[%s] write reg[0x%02x] failed\n", __func__, wbuf[0]);
		return -1;
	}

	return 0;
}

int read_16bit_reg(int fd, unsigned int* reg, unsigned int* val)
{
	//set reg addr to read
	if (write(fd, reg, 2) != 2) {
		printf("[%s] ERROR: write reg[0x%02x%02x] failed\n", __func__, reg[0], reg[1]);
		return -1;
	}

	if (read(fd, val,2) != 2) {
		printf("[%s] ERROR: read reg[0x%02x%02x] failed\n", __func__, reg[0], reg[1]);
		return -1;
	}

	return 0;
}

int write_16bit_reg(int fd, unsigned int* reg, unsigned int* val)
{
	unsigned char wbuf[4];
	wbuf[0] = (*reg >> 8) && 0xff;
	wbuf[1] = *reg & 0xff;

	wbuf[2] = (*val >> 8) && 0xff;
	wbuf[3] = *val & 0xff;

	//write 16bit reg; wbuf[0~1] is reg addr
	if (write(fd, wbuf, 4) != 4) {
		printf("[%s] write reg[0x%02x%02x] failed\n", __func__, wbuf[0], wbuf[1]);
		return -1;
	}

	return  0;
}

