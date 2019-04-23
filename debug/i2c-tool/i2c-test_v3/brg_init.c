#include <stdio.h>
#include "brg_init.h"
#include "i2c_io_ops.h"


int program_register_for_913_ar0144(int bus_id)
{
	/*
	 * 913 slave addr: 7'bit: 0x5d
	 * i2cset -y 1 0x61 0x7 0xBA

	 * ar0144 slave addr: 7'bit: 0x10
	 * i2cset -y 1 0x61 0x9 0x20
	 * i2cset -y 1 0x61 0x11 0x20
	 */

	int i = 3;
	unsigned char wbuf[6] = {0x7, 0xBA/*913*/, 0x9, 0x20, 0x11, 0x20};
	unsigned char rbuf[2];
	unsigned char * pwbuf = wbuf;
	int i2c_addr = DS90UB914_ADDR;
	int status;
	int fd;


	fd = i2c_dev_open(bus_id);
	ioctl(fd,I2C_TENBIT,0);
	// SETTING i2c slave ADDR
	status = ioctl(fd, I2C_SLAVE, i2c_addr);
	if (status < 0)  {
		printf("[%s] ERROR: ioctl(fd, I2C_SLAVE, 0x%02X) failed\n", __func__, i2c_addr);
		close(fd);
		return -1;
	}


	while (i--) {
		if(write(fd, pwbuf, 2) != 2){
			printf("[%s] ERROR: write reg[0x%02x] \n", __func__, *pwbuf);
			return -1;
		}

		read_8bit_reg(fd, pwbuf, rbuf);
		printf("[%s] [%d] Reg[%x] -- 0x%02x\n", __func__, i, pwbuf[0], rbuf[0]);

		pwbuf = pwbuf + 2;
	}

	close(fd);

	return 0;
}
