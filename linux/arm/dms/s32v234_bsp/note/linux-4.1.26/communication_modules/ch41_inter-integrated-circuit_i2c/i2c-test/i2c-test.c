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


int main(int argc, char *argv[])
{
	unsigned char wbuf[4];
	unsigned char rbuf[2];
	int i2c_addr = 0x10;
	int status;
	unsigned short int i;


	if (argc <2) {
		printf("please input the register addr(16bits)\n");
		exit(1);
	}

	// OPENING I2C DEVICE
	int fd = open("/dev/i2c-1", O_RDWR);
	if (fd < 0) {
		printf("ERROR: open(%d) failed\n", fd);
		return -1;
	}


	ioctl(fd,I2C_TENBIT,0);
	// SETTING EEPROM ADDR
	status = ioctl(fd, I2C_SLAVE, i2c_addr);
	if (status < 0)  {
		printf("ERROR: ioctl(fd, I2C_SLAVE, 0x%02X) failed\n", i2c_addr);
		close(fd);
		return -1;
	}


	sscanf(argv[1],"%x", &status);
	wbuf[0]=status >> 8;
	wbuf[1]=status & 0xff;
	if(write(fd,wbuf,2)!=2){
		printf("ERROR: buffer pointer initialization of read() failed\n");
	}

	if (read(fd,rbuf,2) != 2) {
		printf("ERROR: read() failed\n");
		close(fd);

		return -1;

	}

	printf("Reg[%x] -- 0x%02x%02x\n", status, rbuf[0],rbuf[1]);

	close(fd);

}
