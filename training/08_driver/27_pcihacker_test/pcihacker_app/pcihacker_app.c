#include <stdio.h>
#include <stdint.h> //uint16_t ...
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>

#define LED_ON _IO('K',0)
//#define LED_OFF _IO('K',1)
#define LED_OFF _IOW('K',1,int)

#define REG_W "w"
#define REG_R "r"

#define RW_ARG_IDX 1
#define BUS_ARG_IDX 2
#define DEV_ARG_IDX (BUS_ARG_IDX+1)
#define FUN_ARG_IDX (BUS_ARG_IDX+2)
#define REG_ARG_IDX (BUS_ARG_IDX+3)
#define VAL_ARG_IDX (BUS_ARG_IDX+4)
#define ARG_MAG_NUM 7

#define PCI_DEV(bus, dev, fun) (((uint16_t)(bus) << 8) | \
		((uint16_t)(dev) << 3)| \
		(uint16_t)(fun))

const char *pathname = "/dev/pcihacker_drv";

typedef union {
	struct {
		uint16_t fun:3;
		uint16_t dev:5;
		uint16_t bus:8;
	} bits;
	uint16_t u16;
} pci_dev_t;

typedef struct {
	pci_dev_t pci_dev;
	/*uint8_t reg;*/
	uint16_t reg;
	uint16_t val;
	uint32_t read_val;
} pci_test_t;
pci_test_t g_pci_test;


int main(int argc, const char *argv[])
{
	int fd;
	char buf[16];

	if (argc != ARG_MAG_NUM) {
		printf("### error: argc != 6\n");
		return -1;
	}


	fd = open(pathname, O_RDWR, 0664);
	if (fd < 0) {
		printf("### open %s failed: 1) check if insmod moduel, "
			"\t 2) try 'sudo chmod 666 /dev/pcihacker_drv' \n", pathname);
		return -1;
	}

	//atoi: param should be decimal
	g_pci_test.pci_dev.bits.bus = (uint16_t)atoi(argv[BUS_ARG_IDX]);
	g_pci_test.pci_dev.bits.dev = (uint16_t)atoi(argv[DEV_ARG_IDX]);
	g_pci_test.pci_dev.bits.fun = (uint16_t)atoi(argv[FUN_ARG_IDX]);

	g_pci_test.reg = (uint8_t)atoi(argv[REG_ARG_IDX]);
	g_pci_test.val = (uint16_t)atoi(argv[VAL_ARG_IDX]);

	int i;
	for (i=BUS_ARG_IDX; i < BUS_ARG_IDX+5; i++) {
		printf("### argv[%d]=%s\n", i, argv[i]);
	}
	printf("bdf: %x:%x.%x, reg: %x, val: %x\n",
			g_pci_test.pci_dev.bits.bus, g_pci_test.pci_dev.bits.dev,
			g_pci_test.pci_dev.bits.fun, g_pci_test.reg, g_pci_test.val);
	//adb shell lspci -k
	//offset 0: vendor, eg. intel: 0x8086(2-byte)

	printf("### pci_dev: 0x%x, reg: 0x%x, val: 0x%x\n", g_pci_test.pci_dev.u16, g_pci_test.reg, g_pci_test.val);

	if (!strncmp(REG_R, argv[RW_ARG_IDX], 1)) {
		printf("### [read reg]\n");
		// read cfg
		read(fd, (void *)&g_pci_test, sizeof(g_pci_test));
		printf("### pci_dev: 0x%x, reg: 0x%x, val: 0x%x\n", g_pci_test.pci_dev.u16, g_pci_test.reg, g_pci_test.val);
		printf("read_val: %x\n", g_pci_test.read_val);
	} else if (!strncmp(REG_W, argv[RW_ARG_IDX], 1)) {
		printf("### [write reg]\n");
		write(fd, (void *)&g_pci_test, sizeof(g_pci_test));
	} else {
		printf("### reg_wr_flag should be r/w\n");
	}

	/*ioctl(fd, LED_ON);*/
	/*ioctl(fd, LED_OFF, &fd);*/

	return 0;
}
