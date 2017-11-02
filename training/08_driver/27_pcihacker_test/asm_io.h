#ifndef _ASM_IO_H
#define _ASM_IO_H

#include <linux/types.h>

static inline uint8_t asm_in8(uint16_t port)
{
	uint8_t val8;

	__asm__ __volatile__ (
		"inb %1, %0"
		: "=a" (val8)
		: "d" (port)
		);

	return val8;
}

static inline uint16_t asm_in16(uint16_t port)
{
	uint16_t val16;

	__asm__ __volatile__ (
		"inw %1, %0"
		: "=a" (val16)
		: "d" (port)
		);

	return val16;
}

static inline uint32_t asm_in32(uint16_t port)
{
	uint32_t val32;

	__asm__ __volatile__ (
		"inl %1, %0"
		: "=a" (val32)
		: "d" (port)
		);

	return val32;
}

static inline void asm_out8(uint16_t port, uint8_t val8)
{
	__asm__ __volatile__ (
		"outb %1, %0"
		:
		: "d" (port), "a" (val8)
		);
}

static inline void asm_out16(uint16_t port, uint16_t val16)
{
	__asm__ __volatile__ (
		"outw %1, %0"
		:
		: "d" (port), "a" (val16)
		);
}

static inline void asm_out32(uint16_t port, uint32_t val32)
{
	__asm__ __volatile__ (
		"outl %1, %0"
		:
		: "d" (port), "a" (val32)
		);
}
#endif
