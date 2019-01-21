#include "pci_access.h"
#include "asm_io.h"
#include <linux/printk.h>


/* Notes:
 * 1. According to PCI spec, the 2 low bits of PCI_CFG_ADDR are
 *    read-only and must be 0. We have verified it with tests.
 *
 * 2. For pci_read/write16(), the caller must guarantee the reg is
 *    2-bytes aligned; and for pci_read/write32() the reg must be
 *    4-bytes aligned. Otherwise, the behavior is undefined.
 */

uint8_t pci_read8(uint16_t pci_dev, uint8_t reg)
{
	asm_out32(PCI_CFG_ADDR, PCI_ADDR(pci_dev, reg) & (~0x3U));
	return asm_in8(PCI_CFG_DATA | (reg & 0x3));
}

void pci_write8(uint16_t pci_dev, uint8_t reg, uint8_t val)
{
	asm_out32(PCI_CFG_ADDR, PCI_ADDR(pci_dev, reg) & (~0x3U));
	asm_out8(PCI_CFG_DATA | (reg & 0x3), val);
}

uint16_t pci_read16(uint16_t pci_dev, uint8_t reg)
{
	asm_out32(PCI_CFG_ADDR, PCI_ADDR(pci_dev, reg) & (~0x3U));
	return asm_in16(PCI_CFG_DATA | (reg & 0x2));
}

void pci_write16(uint16_t pci_dev, uint8_t reg, uint16_t val)
{
	asm_out32(PCI_CFG_ADDR, PCI_ADDR(pci_dev, reg) & (~0x3U));
	asm_out16(PCI_CFG_DATA | (reg & 0x2), val);
}

uint32_t pci_read32(uint16_t pci_dev, uint8_t reg)
{
	asm_out32(PCI_CFG_ADDR, PCI_ADDR(pci_dev, reg) & (~0x3U));
	return asm_in32(PCI_CFG_DATA);
}

void pci_write32(uint16_t pci_dev, uint8_t reg, uint32_t val)
{
	asm_out32(PCI_CFG_ADDR, PCI_ADDR(pci_dev, reg) & (~0x3U));
	asm_out32(PCI_CFG_DATA, val);
}


static uint8_t pci_device_bar_decode(uint16_t pci_dev, uint8_t  bar_offset, base_addr_reg_t *bar)
{
	uint32_t bar_value_low = pci_read32(pci_dev, bar_offset);
	uint32_t bar_value_high = 0;
	uint64_t bar_value = 0;
	uint32_t encoded_size_low = 0;
	uint32_t encoded_size_high = 0;
	uint64_t encoded_size = 0;
	uint64_t mask;
	uint32_t address_type = PCI_BAR_MMIO_32; //0:32 bits, 2:64 bits
	pci_dev_t dev_addr;

	dev_addr.u16 = pci_dev;
	printk("%x:%x:%x:%x, bar_value_low=0x%x\r\n",
			dev_addr.bits.bus, dev_addr.bits.dev, dev_addr.bits.fun, bar_offset, bar_value_low);


	if (bar_value_low <= 1) {
		bar->type = PCI_BAR_UNUSED;
		return 4;
	}

	// issue size determination command
	pci_write32(pci_dev, bar_offset, 0xFFFFFFFF);
	encoded_size_low = pci_read32(pci_dev, bar_offset);

	bar->type = bar_value_low & 0x1;
	mask = (PCI_BAR_IO == bar->type) ? (uint64_t)~(0x3) : (uint64_t)~(0xf);

	if(bar->type == PCI_BAR_MMIO) { //the BAR that map into 64bits mmio space

		// valid only for mmio
		address_type = (uint32_t)(bar_value_low & 0x6);

		if (address_type == PCI_BAR_MMIO_64) {
			bar_value_high = pci_read32(pci_dev, bar_offset + 4);
			pci_write32(pci_dev, bar_offset + 4, 0xFFFFFFFF);

			encoded_size_high = pci_read32(pci_dev, bar_offset + 4);
			bar_value = MAKE64(bar_value_high, bar_value_low);
			bar->addr = bar_value & mask;
			encoded_size = MAKE64(encoded_size_high, encoded_size_low);
			encoded_size &= mask;
			bar->length = (~encoded_size) + 1;
			pci_write32(pci_dev, bar_offset, bar_value_low); // restore original value
			pci_write32(pci_dev, bar_offset + 4, bar_value_high); // restore original valuie
			return 8;
		} else {
			VMM_ASSERT_EX((address_type == PCI_BAR_MMIO_32), "invalid BAR type(bar=0x%x)\n", bar_value_low);
		}
	}

	//the BAR that map into 32bits mmio or io space
	bar->addr = (uint64_t)bar_value_low & mask;
	encoded_size = MAKE64(0xFFFFFFFF, encoded_size_low);
	encoded_size &= mask;
	bar->length = (~encoded_size) + 1;
	pci_write32(pci_dev, bar_offset, bar_value_low); // restore original value

	if (PCI_BAR_IO == bar->type) {
		bar->length &= 0xFFFF; // IO space in Intel arch can't exceed 64K bytes
	}

	return 4;
}

static void pci_bridge_bar_decode(uint16_t pci_dev, base_addr_reg_t *bar_mmio, base_addr_reg_t *bar_io)
{
	uint32_t memory_base = ((uint32_t)pci_read16(pci_dev, PCI_CFG_BRIDGE_MEM_BASE) << 16) & 0xFFF00000;
	uint32_t memory_limit = ((uint32_t)pci_read16(pci_dev, PCI_CFG_BRIDGE_MEM_LIMIT) << 16) | 0x000FFFFF;
	uint8_t io_base_low = pci_read8(pci_dev, PCI_CFG_BRIDGE_IO_BASE_LOW);
	uint8_t io_limit_low = pci_read8(pci_dev, PCI_CFG_BRIDGE_IO_LIMIT_LOW);
	uint16_t io_base_high = 0;
	uint16_t io_limit_high = 0;
	uint64_t io_base;
	uint64_t io_limit;
	pci_dev_t dev_addr;

	dev_addr.u16 = pci_dev;

	// mmio
	if (memory_limit < memory_base) {
		bar_mmio->type = PCI_BAR_UNUSED;
	} else {
		bar_mmio->type = PCI_BAR_MMIO;
		bar_mmio->addr = (uint64_t)memory_base;
		bar_mmio->length = (uint64_t)(memory_limit - memory_base +1);
	}

	// io
	if (io_base_low == 0 || io_limit_low == 0 || io_limit_low < io_base_low) {
		bar_io->type = PCI_BAR_UNUSED;
	} else if ((io_base_low & 0xF) > 1) {
		bar_io->type = PCI_BAR_UNUSED;
		printk("Reserved IO address capability in bridge (%x:%x:%x) is detected, io_base_low=0x%x\r\n",
			dev_addr.bits.bus, dev_addr.bits.dev, dev_addr.bits.fun, io_base_low);
	} else {
		if ((io_base_low & 0xF) == 1) { // 32 bits IO address
			// update the high 16 bits
			io_base_high = pci_read16(pci_dev, PCI_CFG_BRIDGE_IO_BASE_HIGH);
			io_limit_high = pci_read16(pci_dev, PCI_CFG_BRIDGE_IO_LIMIT_HIGH);
		}

		io_base = (((uint64_t)io_base_high << 16) & 0x00000000FFFF0000ULL) |
			(((uint64_t)io_base_low << 8) & 0x000000000000F000ULL);
		io_limit = (((uint64_t)io_limit_high << 16) & 0x00000000FFFF0000ULL) |
			(((uint64_t)io_limit_low << 8) & 0x000000000000F000ULL) | 0x0000000000000FFFULL;

		bar_io->type = PCI_BAR_IO;
		bar_io->addr = io_base;
		bar_io->length = io_limit - io_base + 1;
	}
}

/*static*/ void pci_cfg_bars_decode(pci_block_device_t *pci_dev_info)
{
	uint32_t bar_idx;
	uint8_t bar_offset = PCI_CFG_BAR_FIRST;
	uint16_t base_class = pci_read8(pci_dev_info->pci_dev, PCI_CFG_BASE_CLASS);
	uint16_t sub_class = pci_read8(pci_dev_info->pci_dev, PCI_CFG_SUB_CLASS);

	if (PCI_IS_BRIDGE(base_class, sub_class)) {
		for(bar_idx = 0; bar_idx < PCI_BRIDGE_BAR_NUM; bar_idx++) {
			// Assumption: according to PCI bridge spec 1.2, host_pci_decode_pci_device_bar() will only return 4 (as 32 bit) for bridge
			// 64 bit mapping is not supported in bridge
			bar_offset += pci_device_bar_decode(pci_dev_info->pci_dev, bar_offset, &(pci_dev_info->bars[bar_idx]));
		}
		pci_bridge_bar_decode(pci_dev_info->pci_dev, &(pci_dev_info->bars[bar_idx]), &(pci_dev_info->bars[bar_idx+1])); // set io range and mmio range
		bar_idx += 2;
	} else {
		for(bar_idx = 0; bar_idx < PCI_DEVICE_BAR_NUM; bar_idx++) {
			if (bar_offset > PCI_CFG_BAR_LAST) { // total bar size is 0x10~0x24
				break;
			}
			bar_offset += pci_device_bar_decode(pci_dev_info->pci_dev, bar_offset, &(pci_dev_info->bars[bar_idx]));
		}
	}

	// set rest bars as unused
	for (; bar_idx < PCI_DEVICE_BAR_NUM; bar_idx++) {
		pci_dev_info->bars[bar_idx].type = PCI_BAR_UNUSED;
	}
}
