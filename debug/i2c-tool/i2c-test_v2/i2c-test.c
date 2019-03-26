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


#define DS90UB914_ADDR  0x61
#define DS90UB913_ADDR  0x5D  //write it into 914 reg addr 0x07
#define AR0144_ADDR 0x10
#define REG_ADDR_BIT8_WR 'w'
#define REG_ADDR_BIT8_RD 'r'
#define REG_ADDR_BIT16_WR 'W'
#define REG_ADDR_BIT16_RD 'R'

#define I2C_BUS_DEV0 "/dev/i2c-0"
#define I2C_BUS_DEV1 "/dev/i2c-1"
#define I2C_BUS_DEV I2C_BUS_DEV0


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

int read_16bit_reg(int fd, unsigned char* reg, unsigned char* rbuf)
{
	//set reg addr to read
	if(write(fd, reg, 2)!=2){
		printf("[%s] ERROR: write reg[0x%02x%02x] failed\n", __func__, reg[0], reg[1]);
		return -1;
	}

	if (read(fd, rbuf,2) != 2) {
		printf("[%s] ERROR: read reg[0x%02x%02x] failed\n", __func__, reg[0], reg[1]);
		return -1;
	}

	return 0;
}

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

	// OPENING I2C DEVICE
	if (bus_id == 0) {
		fd = open(I2C_BUS_DEV0, O_RDWR);
	} else if (bus_id == 1) {
		fd = open(I2C_BUS_DEV1, O_RDWR);
	} else {
		//fd = open(I2C_BUS_DEV2, O_RDWR);
		//todo
	}

	if (fd < 0) {
		printf("[%s] ERROR: open %s (%d) failed\n", I2C_BUS_DEV, __func__,  fd);
		return -1;
	}

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

int main(int argc, char *argv[])
{
	unsigned char wbuf[4];
	unsigned char rbuf[2];
	/*int i2c_addr = 0x10;*/
	int i2c_addr = DS90UB914_ADDR;
	int status, val, reg_addr;
	int bus_id;
	int fd;
	unsigned long i = 0;


	if (argc < 5 || (*argv[2] != REG_ADDR_BIT8_WR && *argv[2] != REG_ADDR_BIT16_WR \
				&& *argv[2] != REG_ADDR_BIT8_RD && *argv[2] != REG_ADDR_BIT16_RD )) {
		printf("please pass the param:\n");
		printf("./i2c-test bus_id w i2c_addr 8bit_register_addr val\n");
		printf("./i2c-test bus_id W i2c_addr 16bit_register_addr val\n");
		printf("./i2c-test bus_id r i2c_addr 8bit_register_addr 0\n");
		printf("./i2c-test bus_id R i2c_addr 16bit_register_addr 0\n");
		printf("write 914 reg[0x61]: ./i2c-test bus_id w 0x61 0x8 0x36\n");
		printf("write 913 reg[0x5d]: ./i2c-test bus_id w 0x5d 0x8 0x36\n");
		printf("write ar0144 reg[0x3270]: ./i2c-test bus_id W 0x10 0x3270 0x8\n");

		exit(1);
	}

	sscanf(argv[1],"%x", &bus_id); //bus id

#if 0
	program_register_for_913_ar0144(bus_id);
#else
#endif
	//sleep(1);

	sscanf(argv[3],"%x", &i2c_addr); //reg
	sscanf(argv[4],"%x", &reg_addr); //reg
	sscanf(argv[5],"%x", &val); //write val



	// OPENING I2C DEVICE
	if (bus_id == 0) {
		fd = open(I2C_BUS_DEV0, O_RDWR);
	} else if (bus_id == 1) {
		fd = open(I2C_BUS_DEV1, O_RDWR);
	} else {
		//fd = open(I2C_BUS_DEV2, O_RDWR);
		//todo
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



	switch (*argv[2]) {
	case REG_ADDR_BIT8_WR: //913 and 914 is 8bit reg

		wbuf[0] = reg_addr & 0xff; //8bit reg
		wbuf[1] = val;

		while (1) {
			//write 8bit reg; first byte is reg addr
			if(write(fd, wbuf, 2) != 2){
				printf("[%s] write reg[0x%02x] failed\n", __func__, wbuf[0]);
				close(fd);
				return -1;
			}

			read_8bit_reg(fd, wbuf, rbuf);

			i++;
			printf("[%s] [%ld] Reg[%x] -- 0x%02x\n", __func__, i, reg_addr, rbuf[0]);
			break;
		}

		break;

	case REG_ADDR_BIT8_RD: //913 and 914 is 8bit reg

		wbuf[0] = reg_addr & 0xff; //8bit reg

		read_8bit_reg(fd, wbuf, rbuf);

		printf("[%s] Reg[%x] -- 0x%02x\n", __func__, reg_addr, rbuf[0]);

		break;

	case REG_ADDR_BIT16_WR: //ar0144 is 16bit reg

		wbuf[0] = reg_addr >> 8;
		wbuf[1] = reg_addr & 0xff;

		wbuf[2] = val >> 8;
		wbuf[3] = val & 0xff;


		while (1) {
			//write 16bit reg; wbuf[0~1] is reg addr
			if(write(fd, wbuf, 4)!=4){
				printf("[%s] write reg[0x%02x%02x] failed\n", __func__, wbuf[0], wbuf[1]);
				close(fd);
				return -1;
			}

			//read reg
			if(write(fd, wbuf, 2)!=2){
				printf("[%s] write reg[0x%02x%02x] failed\n", __func__, wbuf[0], wbuf[1]);
				close(fd);
				return -1;
			}

			if (read(fd, rbuf, 2) != 2) {
				printf("[%s] read reg[0x%02x%02x] failed\n", __func__, wbuf[0], wbuf[1]);
				close(fd);

				return -1;

			}
			i++;
			printf("[%s] Reg[%x] -- 0x%02x%02x\n", __func__, reg_addr, rbuf[0],rbuf[1]);
			break;
		}
		break;

	case REG_ADDR_BIT16_RD: //ar0144 is 16bit reg

		wbuf[0] = reg_addr >> 8;
		wbuf[1] = reg_addr & 0xff;

		//read reg
		if (write(fd, wbuf, 2) != 2 ){
			printf("[%s] write reg[0x%02x%02x] failed\n", __func__, wbuf[0], wbuf[1]);
			close(fd);
			return -1;
		}

		if (read(fd, rbuf, 2) != 2) {
			printf("[%s] read reg[0x%02x%02x] failed\n", __func__, wbuf[0], wbuf[1]);
			close(fd);

			return -1;

		}

		printf("[%s] Reg[%x] -- 0x%02x%02x\n", __func__, reg_addr, rbuf[0],rbuf[1]);
		break;

	default:
		/* sentence; */
		break;
	}

	close(fd);

	return 0;
}
