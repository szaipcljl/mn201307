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
#include <soc/sprd/adi.h>
#endif

#include <asm/gpio.h>
#include <linux/delay.h> 

#define SPRD_FLASH_ON		1
#define SPRD_FLASH_OFF		0

//#undef GPIO_CAM_FLASH_EN
#define GPIO_AW3641_FLASH_TORCH_EN    123  //FLASH TORCH EN
#define GPIO_AW3641_PWM_EN   142  //PWM EN

#define AW3641_TORCH_MODE 0
#define AW3641_FLASH_MODE 1
#define AW3641_OFF_MODE 3

#if 0
#define IFLASH_CURRENT  500  // mA
#define FLASH_ON_CURRENT 13 //300mA  60%
#define HIGHLIGHT_CURRENT 9 //500mA  100%
#else
#define IFLASH_CURRENT  1000  // mA
#define FLASH_ON_CURRENT 15 //400mA   40%
#define HIGHLIGHT_CURRENT 9 //500mA   100%
#endif

static bool is_init = false;
//extern int sprd_3rdparty_gpio_speaker_pa_onoff;
//#define sprd_3rdparty_gpio_speaker_pa_onoff 44
static void aw3641_flash_control(bool enable,u8 flash_mode,u8 current_level)
{
	volatile u16 i,j;
	int ret;

	if (!is_init) {
		ret = gpio_request(GPIO_AW3641_FLASH_TORCH_EN, "cam_torch_flash_mode");
		ret = gpio_request(GPIO_AW3641_PWM_EN, "cam_pwm_en");

		gpio_direction_output(GPIO_AW3641_FLASH_TORCH_EN, 0);
		gpio_direction_output(GPIO_AW3641_PWM_EN, 0);

		is_init = true;
	}	

	if (enable) 
	{
		if(flash_mode==AW3641_TORCH_MODE)
		{
			gpio_set_value(GPIO_AW3641_PWM_EN, 1);
			gpio_set_value(GPIO_AW3641_FLASH_TORCH_EN, 0);
		}
		else
		{
			gpio_set_value(GPIO_AW3641_FLASH_TORCH_EN, 1);
			/*0~8,OVP time is 220ms;9~16,OVP time is 1.3s*/
			/*because of platform flash time out more than 0.6s,so we use 1.3s timeout*/
			for(j=0; j < current_level; j++)
			{
				gpio_set_value(GPIO_AW3641_PWM_EN, 0);
				if(j == 0) 
				{
					udelay(850);
				} 
				else 
				{
					udelay(3);
				}
				gpio_set_value(GPIO_AW3641_PWM_EN, 1);
				udelay(3);
			}
		}
	} 
	else 
	{
		gpio_set_value(GPIO_AW3641_PWM_EN, 0);
		gpio_set_value(GPIO_AW3641_FLASH_TORCH_EN, 0);
		udelay(900);
	}
}

int sprd_torch_on(void)
{
	printk("sprd_troch_on \n");
	aw3641_flash_control(SPRD_FLASH_ON,AW3641_TORCH_MODE, 0);
	return 0;
}

int sprd_flash_on(void)
{
	printk("sprd_flash_on \n");
	aw3641_flash_control(SPRD_FLASH_ON,AW3641_FLASH_MODE,FLASH_ON_CURRENT);// 300mA

	return 0;
}

int sprd_flash_high_light(void)
{
	printk("sprd_flash_high_light \n");
	aw3641_flash_control(SPRD_FLASH_ON,AW3641_FLASH_MODE,HIGHLIGHT_CURRENT);// 500mA

	return 0;
}

int sprd_flash_close(void)
{
	printk("sprd_flash_close \n");
	aw3641_flash_control(SPRD_FLASH_OFF,AW3641_OFF_MODE, 0);

	return 0;
}

int sprd_flash_cfg(struct sprd_flash_cfg_param *param, void *arg)
{
	return 0;
}
