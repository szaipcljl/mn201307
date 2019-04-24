#ifndef _I2C_IO_OPS_H
#define _I2C_IO_OPS_H

#define I2C_BUS_DEV0 "/dev/i2c-0"
#define I2C_BUS_DEV1 "/dev/i2c-1"
#define I2C_BUS_DEV I2C_BUS_DEV0

/*
struct i2c_dev {
	int bus_id;
	char *adapt_dev[10];
	int i2c_addr;
	int fd;
	int reg_type; //16bit or 8bit
};
*/

int i2c_dev_open(int bus_id,int i2c_addr);
int i2c_dev_close(int fd);

int read_8bit_reg(int fd, unsigned char reg, unsigned char *rbuf);
int write_8bit_reg(int fd, unsigned char reg, unsigned char val);

int read_16bit_reg(int fd, unsigned int reg, unsigned int* val);
int write_16bit_reg(int fd, unsigned int reg, unsigned int val);
#endif
