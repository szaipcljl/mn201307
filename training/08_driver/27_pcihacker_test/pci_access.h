#ifndef _PCI_ACCESS_H
#define _PCI_ACCESS_H

#include <linux/types.h>

#define PCI_CFG_ADDR 0xCF8
#define PCI_CFG_DATA 0xCFC
#define PCI_ADDR_ENABLE 0x80000000

#define PCI_CFG_VID				0x00
#define PCI_CFG_DID				0x02
#define PCI_CFG_SUB_CLASS			0x0A
#define PCI_CFG_BASE_CLASS			0x0B
#define PCI_CFG_BAR_FIRST			0x10
#define PCI_CFG_BAR_LAST			0x24

#define PCI_DEVICE_BAR_NUM			6
#define PCI_BRIDGE_BAR_NUM			2

#define PCI_CFG_BRIDGE_IO_BASE_LOW		0x1C
#define PCI_CFG_BRIDGE_IO_LIMIT_LOW		0x1D
#define PCI_CFG_BRIDGE_MEM_BASE			0x20
#define PCI_CFG_BRIDGE_MEM_LIMIT		0x22
#define PCI_CFG_BRIDGE_IO_BASE_HIGH		0x30
#define PCI_CFG_BRIDGE_IO_LIMIT_HIGH		0x32

#define PCI_BAR_MMIO				0
#define PCI_BAR_IO				1
#define PCI_BAR_UNUSED				-1
#define PCI_BAR_MMIO_32				0
#define PCI_BAR_MMIO_64				0x4

/* PCI-to-PCI bridge: class code 0x06, sub class 0x04 */
#define PCI_IS_BRIDGE(base_class, sub_class) ((base_class) == 0x06 && (sub_class) == 0x04)


/*#define print_trace(fmt, ...) printk(fmt, ##__VA_ARGS__);*/

/* according to IA32 spec, shift left/shift right instructions (SAL/SAR/SHL/SHR)
 * treat the "count" as "count % 64" (for 32 bit, it is 32).
 * that is, m << n == m << (n%64), m >> n == m >> (n%64)
 * that is, 1ULL << 64 == 1ULL, while usually we think it is 0 in our code
 */
#define BIT(n) (((n) >= 64U)?0:1ULL<<(n))
#define MASK64_LOW(n) ((BIT(n)) - 1)
#define MASK64_MID(h, l) ((BIT((h) + 1)) - (BIT(l)))
#define MAKE64(high, low) (((uint64_t)(high))<<32 | (((uint64_t)(low)) & MASK64_LOW(32)))

#define print_panic(fmt, ...) printk("PANIC: " fmt, ##__VA_ARGS__)
#define vmm_deadloop(file, line) printk("PANIC: %s:%d\n", file, line)
#define VMM_ASSERT_EX(__condition, msg, ...) \
{ \
	if (!(__condition)) \
	{\
		print_panic(msg, ##__VA_ARGS__); \
		vmm_deadloop(__FILE__, __LINE__); \
	}\
}

typedef union {
	struct {
		uint16_t fun:3;
		uint16_t dev:5;
		uint16_t bus:8;
	} bits;
	uint16_t u16;
} pci_dev_t;

typedef struct {
	uint32_t	type; //PCI_BAR_MMIO, PCI_BAR_IO, PCI_BAR_UNUSED
	uint32_t	pad;
	uint64_t	addr;
	uint64_t	length;
} base_addr_reg_t;

typedef struct {
	uint16_t	pci_dev;
	uint8_t		pad[6];
	base_addr_reg_t	bars[PCI_DEVICE_BAR_NUM];
} pci_block_device_t;



#define PCI_DEV(bus, dev, fun) (((uint16_t)(bus) << 8) | \
				((uint16_t)(dev) << 3)| \
				(uint16_t)(fun))

#define PCI_ADDR(pci_dev, reg)	(PCI_ADDR_ENABLE | \
				((uint32_t)(pci_dev) << 8) | \
				(uint32_t)(reg))

#define PCIE_ADDR_OFFSET(pcie_dev, reg) (((uint32_t)(pcie_dev) << 12) | \
					(uint32_t)(reg))

#define PCIE_BASE_OFFSET(pci_dev) PCIE_ADDR_OFFSET(pci_dev, 0)

uint8_t pci_read8(uint16_t pci_dev, uint8_t reg);

void pci_write8(uint16_t pci_dev, uint8_t reg, uint8_t val);

uint16_t pci_read16(uint16_t pci_dev, uint8_t reg);

void pci_write16(uint16_t pci_dev, uint8_t reg, uint16_t val);

uint32_t pci_read32(uint16_t pci_dev, uint8_t reg);

void pci_write32(uint16_t pci_dev, uint8_t reg, uint32_t val);

void pci_cfg_bars_decode(pci_block_device_t *pci_dev_info);
#endif
