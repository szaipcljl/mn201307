#include <stdio.h>
#include <unistd.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <errno.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include "i2c_io_ops.h"
#include "brg_init.h"

#define AR0144_ADDR 0x10
//#define AR0144_ADDR 0x49

#define REG_ADDR_BIT8_WR 'w'
#define REG_ADDR_BIT8_RD 'r'
#define REG_ADDR_BIT16_WR 'W'
#define REG_ADDR_BIT16_RD 'R'


void print_helper()
{
	printf("please pass the param:\n");
	printf("write 8bit  reg:  ./i2c-test bus_id w i2c_addr 8bit_register_addr val\n");
	printf("read  8bit  reg:  ./i2c-test bus_id r i2c_addr 8bit_register_addr 0\n");
	printf("write 16bit reg:  ./i2c-test bus_id W i2c_addr 16bit_register_addr val\n");
	printf("read  16bit reg:  ./i2c-test bus_id R i2c_addr 16bit_register_addr 0\n\n");

	printf("write 914 reg[0x61]: ./i2c-test bus_id w 0x61 0x8 0x36\n");
	printf("write 913 reg[0x5d]: ./i2c-test bus_id w 0x5d 0x8 0x36\n");
	printf("write ar0144 ir-led reg[0x3270](on:0x100, off:0x0): ./i2c-test bus_id W 0x10 0x3270 0x100\n");

	printf("\nar0144 i2c 7bit_addr: 0x10 or 0x49 (remaped by des)");
}

void exec_i2c_access_request(struct i2c_dev *dev, unsigned int reg_addr, unsigned int val)
{
	int fd = dev->fd;
	unsigned int rval;

	switch (dev->reg_type) {
	case REG_ADDR_BIT8_WR: //913 and 914 is 8bit reg

		reg_addr = reg_addr && 0xff;
		write_8bit_reg(fd, (unsigned char *)&reg_addr, (unsigned char *)&val);

		read_8bit_reg(fd, (unsigned char *)&reg_addr, (unsigned char *)&rval);

		printf("[%s] Reg[%x] -- 0x%02x\n", __func__, reg_addr, rval);
		break;

	case REG_ADDR_BIT8_RD: //913 and 914 is 8bit reg

		reg_addr = reg_addr && 0xff;

		write_8bit_reg(fd, (unsigned char *)&reg_addr, (unsigned char *)&val);
		read_8bit_reg(fd, (unsigned char *)&reg_addr, (unsigned char *)&rval);

		printf("[%s] Reg[%x] -- 0x%02x\n", __func__, reg_addr, rval);
		break;

	case REG_ADDR_BIT16_WR: //ar0144 is 16bit reg

		write_16bit_reg(fd, &reg_addr, &val);
		read_16bit_reg(fd, &reg_addr, &rval);
		break;

	case REG_ADDR_BIT16_RD: //ar0144 is 16bit reg

		read_16bit_reg(fd, &reg_addr, &rval);
		break;

	default:
		/* sentence; */
		break;
	}
}


int main(int argc, char *argv[])
{
	unsigned int val, reg_addr;

	struct i2c_dev dev;


	if (argc < 5 || (*argv[2] != REG_ADDR_BIT8_WR && *argv[2] != REG_ADDR_BIT16_WR \
				&& *argv[2] != REG_ADDR_BIT8_RD && *argv[2] != REG_ADDR_BIT16_RD )) {
		print_helper();

		exit(1);
	}

	sscanf(argv[1],"%x", &dev.bus_id); //bus id

#if 0
	program_register_for_913_ar0144(bus_id);
#else
#endif
	//sleep(1);

	sscanf(argv[3],"%x", &dev.i2c_addr); //reg
	sscanf(argv[4],"%x", &reg_addr); //reg
	sscanf(argv[5],"%x", &val); //write val

	dev.fd = i2c_dev_open(dev.bus_id, dev.i2c_addr);

	exec_i2c_access_request(&dev, reg_addr, val);

	close(dev.fd);

	return 0;
}

