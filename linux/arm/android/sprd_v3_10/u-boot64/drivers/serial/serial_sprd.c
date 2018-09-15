/*
 * (C) Copyright 2004
 */
#include <config.h>
#include <common.h>
#include <asm/io.h>
#include <serial.h>
#include <linux/compiler.h>
#include "serial_sprd.h"

#ifdef CONFIG_SERIAL_MULTI
#warning "SC8800X driver does not support MULTI serials."
#endif

#define CONSOLE_PORT CONFIG_CONS_INDEX
static volatile unsigned char *const port[] = CONFIG_SPRD_UART_PORTS;
#define NUM_PORTS (sizeof(port)/sizeof(port[0]))
unsigned int baudrate = CONFIG_BAUDRATE;
#define ARM_APB_CLK    26000000UL

DECLARE_GLOBAL_DATA_PTR;

static struct sprd_uart_regs *sprd_uart_get_regs(int portnum)
{
	return (struct sprd_uart_regs *) port[portnum];
}

static int sprd_serial_init(void)
{
	struct sprd_uart_regs *regs = sprd_uart_get_regs(CONSOLE_PORT);
	u32 divider = (ARM_APB_CLK + baudrate / 2) / baudrate;
    /* Disable UART*/
    writel(readl(REG_AP_APB_APB_EB) & (~BIT_UART1_EB), REG_AP_APB_APB_EB);
    /*Disable Interrupt */
    writel(0, &regs->uart_ien);
    /* Enable UART*/
    writel(readl(REG_AP_APB_APB_EB) | BIT_UART1_EB, REG_AP_APB_APB_EB);
    /* Set baud rate  */
	writel(LWORD (divider), &regs->uart_ckd0);
    /* Set port for 8 bit, one stop, no parity  */
    writel(UARTCTL_BL8BITS | UARTCTL_SL1BITS, &regs->uart_ctrl0);
    writel(0, &regs->uart_ctrl1);
    writel(0, &regs->uart_ctrl2);
    return 0;
}

static void	sprd_serial_setbrg(void)
{
}

static void sprd_putc (int portnum, char c)
{
	struct sprd_uart_regs *regs = sprd_uart_get_regs(portnum);
	while ( ((readl(&regs->uart_sts1) >> 8) & 0xff ))
		/* wait for room in the tx FIFO */ ;

	writeb(c, &regs->uart_txd);

	/* Ensure the last byte is written successfully */
	while ( ((readl(&regs->uart_sts1) >> 8) & 0xff ));
	return 0;
}

static int sprd_getc (int portnum)
{
	struct sprd_uart_regs *regs = sprd_uart_get_regs(portnum);
	while (!(readl(&regs->uart_sts1) & 0xff))
	/* wait for character to arrive */ ;
	return (readb(&regs->uart_rxd) & 0xff);
}

static int sprd_tstc (int portnum)
{
	struct sprd_uart_regs *regs = sprd_uart_get_regs(portnum);
	/* If receive fifo is empty, return false */
	return (readl(&regs->uart_sts1) & 0xff);
}

static int sprd_serial_getc(void)
{
	return sprd_getc (CONSOLE_PORT);
}

/*
 *  * Test whether a character is in the RX buffer
 *   */
static int sprd_serial_tstc(void)
{
	return sprd_tstc (CONSOLE_PORT);
}
static void	sprd_serial_putc(const char c)
{
	if (c == '\n')
		sprd_putc (CONSOLE_PORT, '\r');

	sprd_putc (CONSOLE_PORT, c);
}

static struct serial_device sprd_serial_drv = {
	.name	= "sprd_serial",
	.start	= sprd_serial_init,
	.stop	= NULL,
	.setbrg	= sprd_serial_setbrg,
	.getc	= sprd_serial_getc,
	.tstc	= sprd_serial_tstc,
	.putc	= sprd_serial_putc,
	.puts	= default_serial_puts,
};

void sprd_uart_initialize(void)
{
	serial_register(&sprd_serial_drv);
}

__weak struct serial_device *default_serial_console(void)
{
	return &sprd_serial_drv;
}

