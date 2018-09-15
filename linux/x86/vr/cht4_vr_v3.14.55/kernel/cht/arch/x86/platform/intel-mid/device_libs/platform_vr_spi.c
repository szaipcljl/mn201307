/*
 * platform_wm5102.c: wm51020 platform data initilization file
 *
 * (C) Copyright 2008 Intel Corporation
 * Author:
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */
#include <linux/gpio.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/fixed.h>
#include <linux/platform_device.h>
#include <linux/sfi.h>
#include <linux/spi/spi.h>
#include <asm/intel-mid.h>
#include <linux/mfd/arizona/pdata.h>


static struct arizona_pdata st_pdata  = {
	.irq_gpio=505,
	.irq_flags = IRQF_TRIGGER_LOW  | IRQF_ONESHOT,
};

static struct spi_board_info __initdata st_board_info = {
	.platform_data = &st_pdata,
};


static void st_sensor_register_spi_dev(void)
{
//    struct spi_board_info spi_info;

    memset(&st_board_info, 0, sizeof(st_board_info));
    //strncpy(st_board_info.modalias, "spi-st", sizeof("spi-st")); // load spi-test driver
    strncpy(st_board_info.modalias, "vr-sensor-spi-ipc", sizeof("vr-sensor-spi-ipc"));  // load vr-spi-driver
    st_board_info.irq = 0 ;
    st_board_info.bus_num = 1;
    st_board_info.chip_select = 0;
    st_board_info.max_speed_hz = 5000000;  //10M 
    st_board_info.platform_data = &st_pdata;
    
    pr_info("SPI bus=%d, name=%16.16s, irq=0x%2x, max_freq=%d, cs=%d, pdata=0x%x\n",
                st_board_info.bus_num,
                st_board_info.modalias,
                st_board_info.irq,
                st_board_info.max_speed_hz,
                st_board_info.chip_select,
		st_board_info.platform_data);

    spi_register_board_info(&st_board_info, 1);
}


static int __init st_sensor_platform_init(void)
{
	int ret;

	pr_info("in %s\n", __func__);


	st_sensor_register_spi_dev();

	return 0;
}
device_initcall(st_sensor_platform_init);


