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
#define ARG_MAX 7

#define PCI_DEV(bus, dev, fun) (((uint16_t)(bus) << 8) | \
		((uint16_t)(dev) << 3)| \
		(uint16_t)(fun))

const char *pathname = "/dev/pcihacker_drv";


struct pci_test_t {
	uint16_t pci_dev;
	/*uint8_t reg;*/
	uint16_t reg;
	uint16_t val;
};
typedef struct pci_test_t pci_test;
pci_test g_pci_test;


int main(int argc, const char *argv[])
{
	int fd;
	char buf[16];

	if (argc != ARG_MAX) {
		printf("### error: argc != 6\n");
		return -1;
	}


	fd = open(pathname, O_RDWR, 0664);
	if (fd < 0) {
		printf("### open %s failed: try 'sudo chmod 666 /dev/pcihacker_drv' \n", pathname);
		return -1;
	}

#if 1
	//atoi: param should be decimal 
	uint16_t bus = (uint16_t)atoi(argv[BUS_ARG_IDX]);
	uint16_t dev = (uint16_t)atoi(argv[DEV_ARG_IDX]);
	uint16_t func =	(uint16_t)atoi(argv[FUN_ARG_IDX]);

	g_pci_test.pci_dev = PCI_DEV(bus, dev, func);
	g_pci_test.reg = (uint8_t)atoi(argv[REG_ARG_IDX]);
	g_pci_test.val = (uint16_t)atoi(argv[VAL_ARG_IDX]);

	int i;
	for (i=BUS_ARG_IDX; i < BUS_ARG_IDX+5; i++) {
		printf("### argv[%d]=%s\n", i, argv[i]);
	}
	printf("bus: %x, dev: %x, func: %x, reg: %x, val: %x\n",
			bus, dev, func, g_pci_test.reg, g_pci_test.val);
#else
	//adb shell lspci -k
	g_pci_test.pci_dev = PCI_DEV(0, 0xd, 0);
	g_pci_test.reg = 0x0; //vendor 0x8086(2-byte)
	g_pci_test.val = 0;
#endif
	printf("### pci_dev=%x, reg=%x, val=%x\n", g_pci_test.pci_dev, g_pci_test.reg, g_pci_test.val);

	if (!strncmp(REG_R, argv[RW_ARG_IDX], 1)) {
		printf("### [read reg]\n");
		// read cfg
		read(fd, (void *)&g_pci_test, sizeof(g_pci_test));
	} else if (!strncmp(REG_W, argv[RW_ARG_IDX], 1)) {
		printf("### [write reg]\n");
		write(fd, (void *)&g_pci_test, sizeof(g_pci_test));
	} else {
		printf("### reg_wr_flag should be r/w\n");
	}

	ioctl(fd, LED_ON);
	ioctl(fd, LED_OFF, &fd);

	return 0;
}
