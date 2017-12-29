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

#define SPRD_PWM0_CTRL_OFST                       0xEC
#define SPRD_PWM0_PATTERN_HIGHT_OFST              0x10
#define SPRD_PWM0_PATTERT_LOW_OFST                0xC
#define SPRD_PWM0_TONE_OFST                       0x8
#define SPRD_PWM0_RATION_OFST                     0x4
#define SPRD_WHTLED_CTRL_OFST                     0xF0

extern int sci_efuse_ib_trim_get(unsigned int *p_cal_data);

unsigned int ib_trim_cal_data = 0;
static int init_flag = 1;

#define TORCH_ON_CURRENT  20
#define FLASH_ON_CURRENT  50
#define HIGHLIGH_CURRENT  200

#define WHTLED_IB_NUMS 4
#define PER_STEP_CURRENT (0.625)
#define FIRST_STEP_CURRENT (5)
#define MAX_CURRENT_PER_WHTLED (45)

unsigned int calculate_steps_for_whtled(unsigned int cur_current)
{
	unsigned int cur_step=0;
	unsigned int current_per_whtled;

	if(cur_current<=WHTLED_IB_NUMS * FIRST_STEP_CURRENT)
	{
		cur_step=0;
	}
	else if(cur_current >=WHTLED_IB_NUMS * MAX_CURRENT_PER_WHTLED)
	{
		cur_step=0x3f;
	}
	else
	{
		current_per_whtled=cur_current*1000/WHTLED_IB_NUMS;
		printk("***current_per_whtled=%d\n",current_per_whtled);
		cur_step = (current_per_whtled - FIRST_STEP_CURRENT*1000)/625;
	}
	printk("***cur_step=0x%x\n",cur_step);

	return cur_step;
}

int sprd_torch_on(void)
{
	unsigned int step=0;

	step = calculate_steps_for_whtled(TORCH_ON_CURRENT);

	if (init_flag && sci_efuse_ib_trim_get(&ib_trim_cal_data)) {
	/*
		1. set ib_trim_cal_data to WHTLED_CTRL的IB_TRIM（0x400388F0 [15:9]）
		2. set IB_TRIM_EM_SEL of RGB_CTRL to 0（0x400388EC [11]）
	*/
		sci_adi_clr(ANA_CTL_GLB_BASE + SPRD_WHTLED_CTRL_OFST, 0x7F << 9);
		sci_adi_set(ANA_CTL_GLB_BASE + SPRD_WHTLED_CTRL_OFST, (ib_trim_cal_data & 0x7F) << 9);
		sci_adi_clr(ANA_CTL_GLB_BASE + SPRD_PWM0_CTRL_OFST, (0x1 << 11));
		init_flag = 0;
		printk("parallel sprd_flash_on trim = %d\n", ib_trim_cal_data);
	}

	printk("parallel sprd_flash_on \n");
	/*ENABLE THE PWM0 CONTROLLTER: RTC_PWM0_EN=1 & PWM0_EN=1*/
	sci_adi_clr(ANA_CTL_GLB_BASE + SPRD_PWM0_CTRL_OFST, 0xFFFF);
	sci_adi_set(ANA_CTL_GLB_BASE + SPRD_PWM0_CTRL_OFST, 0xC005);

	/*SET PWM0 PATTERN HIGH*/
	sci_adi_set(ANA_PWM_BASE + SPRD_PWM0_PATTERN_HIGHT_OFST, 0xFFFF);

	/*SET PWM0 PATTERN LOW*/
	sci_adi_set(ANA_PWM_BASE + SPRD_PWM0_PATTERT_LOW_OFST, 0xFFFF);

	/*TONE DIV USE DEFAULT VALUE*/
	sci_adi_clr(ANA_PWM_BASE + SPRD_PWM0_TONE_OFST, 0xFFFF);

	/*SET PWM0 DUTY RATIO = 100%: MOD=FF & DUTY=FF*/
	sci_adi_set(ANA_PWM_BASE + SPRD_PWM0_RATION_OFST, 0xFFFF);

	/*ENABLE PWM0 OUTPUT: PWM0_EN=1*/
	sci_adi_set(ANA_PWM_BASE, 0x100);

	printk("sprd_troch_on \n");
	sci_adi_clr(ANA_CTL_GLB_BASE + SPRD_WHTLED_CTRL_OFST, 0xFFFF);
	sci_adi_set(ANA_CTL_GLB_BASE + SPRD_WHTLED_CTRL_OFST, 0xC000|(step <<1));   //20mA
	/*ENABLE WHTLED*/
	sci_adi_clr(ANA_CTL_GLB_BASE + SPRD_WHTLED_CTRL_OFST, 0x1);
	return 0;
}

int sprd_flash_on(void)
{
	unsigned int step=0;

	step = calculate_steps_for_whtled(FLASH_ON_CURRENT);

	if (init_flag && sci_efuse_ib_trim_get(&ib_trim_cal_data)) {
	/*
		1. set ib_trim_cal_data to WHTLED_CTRL的IB_TRIM（0x400388F0 [15:9]）
		2. set IB_TRIM_EM_SEL of RGB_CTRL to 0（0x400388EC [11]）
	*/
		sci_adi_clr(ANA_CTL_GLB_BASE + SPRD_WHTLED_CTRL_OFST, 0x7F << 9);
		sci_adi_set(ANA_CTL_GLB_BASE + SPRD_WHTLED_CTRL_OFST, (ib_trim_cal_data & 0x7F) << 9);
		sci_adi_clr(ANA_CTL_GLB_BASE + SPRD_PWM0_CTRL_OFST, (0x1 << 11));
		init_flag = 0;
		printk("parallel sprd_flash_on trim = %d\n", ib_trim_cal_data);
	}

	printk("parallel sprd_flash_on \n");
	/*ENABLE THE PWM0 CONTROLLTER: RTC_PWM0_EN=1 & PWM0_EN=1*/
	sci_adi_clr(ANA_CTL_GLB_BASE + SPRD_PWM0_CTRL_OFST, 0xFFFF);
	sci_adi_set(ANA_CTL_GLB_BASE + SPRD_PWM0_CTRL_OFST, 0xC005);

	/*SET PWM0 PATTERN HIGH*/
	sci_adi_set(ANA_PWM_BASE + SPRD_PWM0_PATTERN_HIGHT_OFST, 0xFFFF);

	/*SET PWM0 PATTERN LOW*/
	sci_adi_set(ANA_PWM_BASE + SPRD_PWM0_PATTERT_LOW_OFST, 0xFFFF);

	/*TONE DIV USE DEFAULT VALUE*/
	sci_adi_clr(ANA_PWM_BASE + SPRD_PWM0_TONE_OFST, 0xFFFF);

	/*SET PWM0 DUTY RATIO = 100%: MOD=FF & DUTY=FF*/
	sci_adi_set(ANA_PWM_BASE + SPRD_PWM0_RATION_OFST, 0xFFFF);

	/*ENABLE PWM0 OUTPUT: PWM0_EN=1*/
	sci_adi_set(ANA_PWM_BASE, 0x100);

	/*SET LOW LIGHT */
	sci_adi_clr(ANA_CTL_GLB_BASE + SPRD_WHTLED_CTRL_OFST, 0xFFFF);
	sci_adi_set(ANA_CTL_GLB_BASE + SPRD_WHTLED_CTRL_OFST, 0xC000|(step <<1));   //50mA

	/*ENABLE WHTLED*/
	sci_adi_clr(ANA_CTL_GLB_BASE + SPRD_WHTLED_CTRL_OFST, 0x1);
	return 0;
}

int sprd_flash_high_light(void)
{
	unsigned int step=0;

	step = calculate_steps_for_whtled(HIGHLIGH_CURRENT);

	printk("parallel sprd_flash_high_light \n");
	/*SET HIGH LIGHT*/
	sci_adi_clr(ANA_CTL_GLB_BASE + SPRD_WHTLED_CTRL_OFST, 0xFFFF);
	sci_adi_set(ANA_CTL_GLB_BASE + SPRD_WHTLED_CTRL_OFST, 0xC000|(step <<1));
	/*ENABLE WHTLED*/
	sci_adi_clr(ANA_CTL_GLB_BASE + SPRD_WHTLED_CTRL_OFST, 0x1);
	return 0;
}

int sprd_flash_close(void)
{
	printk("parallel sprd_flash_close \n");

	/*DISABLE WHTLED*/
	sci_adi_clr(ANA_CTL_GLB_BASE + SPRD_WHTLED_CTRL_OFST, 0xFFFF);
	sci_adi_set(ANA_CTL_GLB_BASE + SPRD_WHTLED_CTRL_OFST, 0x1);

	/*DISABLE THE PWM0 CONTROLLTER: RTC_PWM0_EN=0 & PWM0_EN=0*/
	sci_adi_clr(ANA_CTL_GLB_BASE + SPRD_PWM0_CTRL_OFST, 0xC000);

	/*ENABLE PWM0 OUTPUT: PWM0_EN=0*/
	sci_adi_clr(ANA_PWM_BASE, 0x100);
	return 0;
}

int sprd_flash_cfg(struct sprd_flash_cfg_param *param, void *arg)
{
	return 0;
}
