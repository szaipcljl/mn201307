#include <stdio.h>
#include <stdint.h>

#define PCI_DEV(bus, dev, fun) (((uint16_t)(bus) << 8) | \
		((uint16_t)(dev) << 3)| \
		(uint16_t)(fun))

typedef union {
	struct {
		uint16_t fun:3;
		uint16_t dev:5;
		uint16_t bus:8;
	} bits;
	uint16_t u16;
} pci_dev_t;

void union_test()
{
	printf("********** union test **********\n");
	pci_dev_t pci_dev;
	pci_dev.bits.bus = (uint16_t)0x1;
	pci_dev.bits.dev = (uint16_t)0xd;
	pci_dev.bits.fun = (uint16_t)0x3;

	printf("pci_dev(hx) : %hx\n", pci_dev.u16);
	printf("pci_dev(hhx): %hhx\n", pci_dev.u16);
	printf("bus(hx)     : %hx\n", pci_dev.bits.bus);


}

void char_test()
{
	char c = 0xf0;

	printf("********** char test **********\n");
	printf("hhx: 0x%hhx\n", c);
	printf("hx : 0x%hx\n", c);
	printf("x  : 0x%x\n", c);
}


int main(int argc, const char *argv[])
{

	char_test();
	union_test();

	return 0;
}
