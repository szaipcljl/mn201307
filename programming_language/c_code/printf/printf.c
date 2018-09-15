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

void point_test()
{
	unsigned long a;
	unsigned long *b;

	a = 0x12345678;
	b = &a;
	printf("p  : %p\n", b);
	printf("lx : 0x%lx\n", b);
	printf("llx: 0x%llx(uint64_t)\n", (uint64_t)b);
	printf("llx: 0x%llx(unsigned int)\n", (unsigned int)b);
}


int main(int argc, const char *argv[])
{

	char_test();
	union_test();
	point_test();

	// # The result is converted to an "alternative form". ... For x (or X) conversion,
	// a nonzero result has 0x (or 0X) prefixed to it
	printf("%#x\n", 128);

	return 0;
}
