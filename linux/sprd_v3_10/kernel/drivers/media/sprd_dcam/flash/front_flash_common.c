/*
 * Copyright (C) 2012 Spreadtrum Communications Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#ifndef CONFIG_64BIT
#include <soc/sprd/hardware.h>
#include <soc/sprd/board.h>
#endif
#include <soc/sprd/adi.h>
#include "../common/parse_hwinfo.h"


#include <asm/gpio.h>
#define SPRD_FLASH_ON		1
#define SPRD_FLASH_OFF		0

#define GPIO_FRONT_CAM_FLASH_EN   236

static bool is_init = false;

int sprd_front_flash_on(void)
{
	printk("sprd_front_flash_on \n");
	if (!is_init) 
	{
		gpio_request(GPIO_FRONT_CAM_FLASH_EN,"cam_front_flash_en");
		gpio_direction_output(GPIO_FRONT_CAM_FLASH_EN, 0);
		is_init = true;
	}
	
	gpio_set_value(GPIO_FRONT_CAM_FLASH_EN, 1);
	
	return 0;
}

int sprd_front_flash_high_light(void)
{
	printk("sprd_front_flash_high_light \n");
	
	gpio_set_value(GPIO_FRONT_CAM_FLASH_EN, 1);
	
	return 0;
}

int sprd_front_flash_close(void)
{
	printk("sprd_front_flash_close \n");
	
	gpio_set_value(GPIO_FRONT_CAM_FLASH_EN, 0);
	
	return 0;
}
