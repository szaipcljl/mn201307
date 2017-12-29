/* drivers/video/sprdfb/lcd_otm8019a_mipi.c
 *
 * Support for otm8019a mipi LCD device
 *
 * Copyright (C) 2010 Spreadtrum
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

#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/delay.h>
#include "../sprdfb_panel.h"
//#define printk printf

#define  LCD_DEBUG
//#define THREE_LANE_SUPPORT

#ifdef LCD_DEBUG
#define LCD_PRINT printk
#else
#define LCD_PRINT(...)
#endif

#define MAX_DATA   48

typedef struct LCM_Init_Code_tag {
	unsigned int tag;
	unsigned char data[MAX_DATA];
}LCM_Init_Code;

typedef struct LCM_force_cmd_code_tag{
	unsigned int datatype;
	LCM_Init_Code real_cmd_code;
}LCM_Force_Cmd_Code;

#define LCM_TAG_SHIFT 24
#define LCM_TAG_MASK  ((1 << 24) -1)
#define LCM_SEND(len) ((1 << LCM_TAG_SHIFT)| len)
#define LCM_SLEEP(ms) ((2 << LCM_TAG_SHIFT)| ms)
//#define ARRAY_SIZE(array) ( sizeof(array) / sizeof(array[0]))

#define LCM_TAG_SEND  (1<< 0)
#define LCM_TAG_SLEEP (1 << 1)

static LCM_Force_Cmd_Code rd_prep_code[]={
	{0x37, {LCM_SEND(2), {0x5, 0}}},
};

static LCM_Force_Cmd_Code rd_prep_code_1[]={
	{0x37, {LCM_SEND(2), {0x1, 0}}},
};

static LCM_Init_Code sleep_in[] =  {
{LCM_SEND(1), {0x28}},
{LCM_SLEEP(10)},
{LCM_SEND(1), {0x10}},
{LCM_SLEEP(120)},
};

static LCM_Init_Code sleep_out[] =  {
{LCM_SEND(1), {0x11}},
{LCM_SLEEP(120)},
{LCM_SEND(1), {0x29}},
{LCM_SLEEP(20)},
};

static LCM_Init_Code init_data[] = {
{LCM_SEND(2),{0x00,0x00}},
{LCM_SEND(6),{4,0,0xFF,0x80,0x19,0x01}},

{LCM_SEND(2),{0x00,0x80}},
{LCM_SEND(5),{3,0,0xFF,0x80,0x19}},

/////////////////////////////////
{LCM_SEND(2),{0x00,0x80}}, 
{LCM_SEND(2),{0xC4,0x30}}, //·ÅµçŽúÂë
{LCM_SLEEP(10)},
{LCM_SEND(2),{0x00,0x8A}}, 
{LCM_SEND(2),{0xC4,0x40}}, //·ÅµçŽúÂë
{LCM_SLEEP(10)},
/////////////////////////////////

{LCM_SEND(2),{0x00,0xB4}},
{LCM_SEND(2),{0xC0,0x00}},
{LCM_SEND(2),{0x00,0x82}},
{LCM_SEND(2),{0xC5,0xB0}},

{LCM_SEND(2),{0x00,0x90}},
{LCM_SEND(5),{3,0,0xC5,0x4e,0x87}},

{LCM_SEND(2),{0x00,0x00}},
{LCM_SEND(5),{3,0,0xD8,0x87,0x87}},

{LCM_SEND(2),{0x00,0x00}},
{LCM_SEND(2),{0xD9,0x50}},
{LCM_SEND(2),{0x00,0xA1}},
{LCM_SEND(2),{0xC1,0x08}},
{LCM_SEND(2),{0x00,0x89}},
{LCM_SEND(2),{0xC4,0x08}},
{LCM_SEND(2),{0x00,0xA3}},
{LCM_SEND(2),{0xC0,0x00}},
{LCM_SEND(2),{0x00,0x81}},
{LCM_SEND(2),{0xC4,0x80}},/*83*/
{LCM_SEND(2),{0x00,0x92}},
{LCM_SEND(2),{0xC5,0x01}},
{LCM_SEND(2),{0x00,0xB1}},
{LCM_SEND(2),{0xC5,0xF8}},/*a9*/
{LCM_SEND(2),{0x00,0xC7}},
{LCM_SEND(2),{0xCF,0x02}},
{LCM_SEND(2),{0x00,0x90}},
{LCM_SEND(2),{0xB3,0x02}},
{LCM_SEND(2),{0x00,0x92}},
{LCM_SEND(2),{0xB3,0x45}},
{LCM_SEND(2),{0x00,0x80}},
{LCM_SEND(12),{10,0,0xC0,0x00,0x58,0x00,0x15,0x15,0x00,0x58,0x15,0x15}},
{LCM_SEND(2),{0x00,0x90}},
{LCM_SEND(9),{7,0,0,0xC0,0x00,0x15,0x00,0x00,0x00,0x03}},

{LCM_SEND(2),{0x00,0x80}},
{LCM_SEND(15),{13,0,0xCE,0x87,0x03,0x00,0x86,0x03,0x00,0x85,0x03,0x00,0x84,0x03,0x00}},
{LCM_SEND(2),{0x00,0x90}},
{LCM_SEND(17),{15,0,0xCE,0x33,0x52,0x00,0x33,0x53,0x00,0x33,0x54,0x00,0x33,0x55,0x00,0x00,0x00}},
{LCM_SEND(2),{0x00,0xA0}},
{LCM_SEND(17),{15,0,0xCE,0x38,0x05,0x03,0x56,0x00,0x00,0x00,0x38,0x04,0x03,0x57,0x00,0x00,0x00}},
{LCM_SEND(2),{0x00,0xB0}},
{LCM_SEND(17),{15,0,0xCE,0x38,0x03,0x03,0x58,0x00,0x00,0x00,0x38,0x02,0x03,0x59,0x00,0x00,0x00}},
{LCM_SEND(2),{0x00,0xC0}},
{LCM_SEND(17),{15,0,0xCE,0x38,0x01,0x03,0x5A,0x00,0x00,0x00,0x38,0x00,0x03,0x5C,0x00,0x00,0x00}},
{LCM_SEND(2),{0x00,0xD0}},
{LCM_SEND(17),{15,0,0xCE,0x30,0x00,0x03,0x5C,0x00,0x00,0x00,0x30,0x01,0x03,0x5D,0x00,0x00,0x00}},

{LCM_SEND(2),{0x00,0xC3}},
{LCM_SEND(12),{10,0,0xCB,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01}},

{LCM_SEND(2),{0x00,0xDA}},
{LCM_SEND(8),{6,0,0xCB,0x01,0x01,0x01,0x01,0x01}},

{LCM_SEND(2),{0x00,0xE0}},
{LCM_SEND(6),{4,0,0xCB,0x01,0x01,0x01}},

{LCM_SEND(2),{0x00,0x83}},
{LCM_SEND(10),{8,0,0xCC,0x03,0x01,0x09,0x0B,0x0D,0x0F,0x05}},

{LCM_SEND(2),{0x00,0x90}},
{LCM_SEND(2),{0xCC,0x07}},

{LCM_SEND(2),{0x00,0xA0}},
{LCM_SEND(11),{9,0,0xCC,0x08,0x06,0x10,0x0e,0x0c,0x0a,0x02,0x04}},

{LCM_SEND(2),{0x00,0xB3}},
{LCM_SEND(10),{8,0,0xCC,0x06,0x08,0x0A,0x10,0x0E,0x0C,0x04}},

{LCM_SEND(2),{0x00,0xC0}},
{LCM_SEND(2),{0xCC,0x02}},

{LCM_SEND(2),{0x00,0xD0}},
{LCM_SEND(11),{9,0,0xCC,0x01,0x03,0x0b,0x0d,0x0f,0x09,0x07,0x05}},

{LCM_SEND(2),{0x00,0x00}},
{LCM_SEND(23),{21,0,0xE1,0x00,0x01,0x03,0x0b,0x1c,0x38,0x4f,0x97,0x8b,0xa1, 0x66,0x56,0x70,0x5d,0x63,0x5b,0x55,0x49,0x43,0x00}},

{LCM_SEND(2),{0x00,0x00}},
{LCM_SEND(23),{21,0,0xE2,0x00,0x01,0x03,0x0b,0x1c,0x38,0x4f,0x96,0x8a,0xa1, 0x66,0x56,0x6f,0x5c,0x62,0x5c,0x56,0x49,0x42,0x00}},

{LCM_SEND(2),{0x00,0x80}},
{LCM_SEND(2),{0xC4,0x30}},

{LCM_SEND(2),{0x00,0x80}},
{LCM_SEND(5),{3,0,0xC1,0x03,0x33}},

{LCM_SEND(2),{0x00,0xA0}},
{LCM_SEND(2),{0xC1,0xE8}},

{LCM_SEND(2),{0x00,0x90}},
{LCM_SEND(2),{0xB6,0xB4}},

{LCM_SEND(2),{0x00,0xC6}},
{LCM_SEND(2),{0xB0,0x03}},

{LCM_SEND(2),{0x00,0x81}},
{LCM_SEND(2),{0xC5,0x66}},

{LCM_SEND(2),{0x00,0xB6}},
{LCM_SEND(2),{0xF5,0x06}},

/////////////////////////////////////////////////
//add
{LCM_SEND(2),{0x00,0x80}},
{LCM_SEND(2),{0xC4,0x30}},	

{LCM_SEND(2),{0x00,0x98}},
{LCM_SEND(2),{0xC0,0x00}},	//

{LCM_SEND(2),{0x00,0xa9}},
{LCM_SEND(2),{0xC0,0x0A}},	//0x06

{LCM_SEND(2),{0x00,0xb0}},
{LCM_SEND(6),{4,0,0xC1,0x20,0x00,0x00}}, //

{LCM_SEND(2),{0x00,0xe1}},
{LCM_SEND(5),{3,0,0xC0,0x40,0x30}}, //0x40,0x18

{LCM_SEND(2),{0x00,0x80}},
{LCM_SEND(5),{3,0,0xC1,0x03,0x44}},/*c181=33*/

{LCM_SEND(2),{0x00,0xA0}},
{LCM_SEND(2),{0xC1,0xe8}},

{LCM_SEND(2),{0x00,0x90}},
{LCM_SEND(2),{0xb6,0xb4}},	//command fial

{LCM_SLEEP(10)},

{LCM_SEND(2),{0x00,0x00}},
{LCM_SEND(2),{0xfb,0x01}},
/////////////////////////////////////////////////

{LCM_SEND(2),{0x00,0x00}},
{LCM_SEND(6),{4,0,0xFF,0xFF,0xFF,0xFF}},

{LCM_SEND(2),{0x11,0x00}}, 
{LCM_SLEEP(120),0},

{LCM_SEND(2),{0x29,0x00}}, 
{LCM_SLEEP(10)},

};

static int32_t otm8019a_mipi_init(struct panel_spec *self)
{
	int32_t i;
	LCM_Init_Code *init = init_data;
	unsigned int tag;

	mipi_set_cmd_mode_t mipi_set_cmd_mode =
		self->info.mipi->ops->mipi_set_cmd_mode;
	mipi_gen_write_t mipi_gen_write = self->info.mipi->ops->mipi_gen_write;
	mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;

	LCD_PRINT("otm8019a_mipi_init\n");

	mipi_set_cmd_mode();
	mipi_eotp_set(1,0);

	for (i = 0; i < ARRAY_SIZE(init_data); i++) {
		tag = (init->tag >> 24);
		if (tag & LCM_TAG_SEND) {
			mipi_gen_write(init->data, (init->tag & LCM_TAG_MASK));
			udelay(20);
		} else if (tag & LCM_TAG_SLEEP) {
			mdelay((init->tag & LCM_TAG_MASK));
		}
		init++;
	}
	mipi_eotp_set(1,1);
	return 0;
}

static uint32_t otm8019a_readid(struct panel_spec *self)
{
	int32_t i = 0;
	uint32_t j =0;
	LCM_Force_Cmd_Code * rd_prepare = rd_prep_code;
	uint8_t read_data[5] = {0};
	int32_t read_rtn = 0;
	unsigned int tag = 0;

	mipi_set_cmd_mode_t mipi_set_cmd_mode =
		self->info.mipi->ops->mipi_set_cmd_mode;
	mipi_force_write_t mipi_force_write = self->info.mipi->ops->mipi_force_write;
	mipi_force_read_t mipi_force_read = self->info.mipi->ops->mipi_force_read;
	mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;

	LCD_PRINT("lcd_otm8019a_mipi read id!\n");

	mipi_set_cmd_mode();
	mipi_eotp_set(1, 0);

	for (j = 0; j < 4; j++) {
		rd_prepare = rd_prep_code;
		for (i = 0; i < ARRAY_SIZE(rd_prep_code); i++) {
			tag = (rd_prepare->real_cmd_code.tag >> 24);
			if (tag & LCM_TAG_SEND)
				mipi_force_write(rd_prepare->datatype,
					rd_prepare->real_cmd_code.data,
					(rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
			else if (tag & LCM_TAG_SLEEP)
				mdelay((rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
			rd_prepare++;
		}

		read_rtn = mipi_force_read(0xa1, 5, (uint8_t *)read_data);
		LCD_PRINT("lcd_otm8019a_mipi read id 0xa1 value is 0x%x, 0x%x, 0x%x, 0x%x, 0x%x!\n",
			read_data[0], read_data[1], read_data[2], read_data[3],
			read_data[4]);

		mipi_eotp_set(1, 1);

		if ((0x01 == read_data[0]) && (0x8b == read_data[1]) &&
			(0x80 == read_data[2]) && (0x19 == read_data[3]) &&
			(0xff == read_data[4])) {
			LCD_PRINT("lcd_otm8019a_mipi read id success!\n");
			return 0x8019;
		}
	}

	printk(KERN_ERR "lcd_otm8019a_mipi identify fail!\n");
	return 0;
}

static uint32_t otm8019a_readpowermode(struct panel_spec *self)
{
	int32_t i = 0;
	uint32_t j =0;
	LCM_Force_Cmd_Code * rd_prepare = rd_prep_code_1;
	uint8_t read_data[1] = {0};
	int32_t read_rtn = 0;
	unsigned int tag = 0;

	mipi_force_write_t mipi_force_write = self->info.mipi->ops->mipi_force_write;
	mipi_force_read_t mipi_force_read = self->info.mipi->ops->mipi_force_read;
	mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;

	pr_debug("lcd_otm8019a_mipi read power mode!\n");
	mipi_eotp_set(0, 1);
	for (j = 0; j < 4; j++) {
		rd_prepare = rd_prep_code_1;
		for (i = 0; i < ARRAY_SIZE(rd_prep_code_1); i++) {
			tag = (rd_prepare->real_cmd_code.tag >> 24);
			if (tag & LCM_TAG_SEND)
				mipi_force_write(rd_prepare->datatype, rd_prepare->real_cmd_code.data,
					(rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
			else if (tag & LCM_TAG_SLEEP)
				msleep((rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
			rd_prepare++;
		}
		read_rtn = mipi_force_read(0x0A, 1, (uint8_t *)read_data);
		/*printk("lcd_otm80193a_mipi read power mode 0x0A value is 0x%x!,
			read result(%d)\n", read_data[0], read_rtn);*/
		if ((0x9c == read_data[0])  && (0 == read_rtn)) {
			pr_debug("lcd_otm8019a_mipi read power mode success!\n");
			mipi_eotp_set(1, 1);
			return 0x9c;
		}
	}

	printk(KERN_ERR "lcd_otm8019a_mipi read power mode fail!0x0A value is 0x%x!, read result(%d)\n",
		read_data[0], read_rtn);
	mipi_eotp_set(1, 1);
	return 0x0;
}

static int32_t otm8019a_check_esd(struct panel_spec *self)
{
	uint32_t power_mode;

#ifndef FB_CHECK_ESD_IN_VFP
	mipi_set_lp_mode_t mipi_set_data_lp_mode =
		self->info.mipi->ops->mipi_set_data_lp_mode;
	mipi_set_hs_mode_t mipi_set_data_hs_mode =
		self->info.mipi->ops->mipi_set_data_hs_mode;
	mipi_set_lp_mode_t mipi_set_lp_mode = self->info.mipi->ops->mipi_set_lp_mode;
	mipi_set_hs_mode_t mipi_set_hs_mode = self->info.mipi->ops->mipi_set_hs_mode;
	uint16_t work_mode = self->info.mipi->work_mode;
#endif

	pr_debug("otm8019a_check_esd!\n");
#ifndef FB_CHECK_ESD_IN_VFP
	if (SPRDFB_MIPI_MODE_CMD == work_mode)
		mipi_set_lp_mode();
	else
		mipi_set_data_lp_mode();
#endif
	power_mode = otm8019a_readpowermode(self);
	//power_mode = 0x0;
#ifndef FB_CHECK_ESD_IN_VFP
	if (SPRDFB_MIPI_MODE_CMD == work_mode)
		mipi_set_hs_mode();
	else
		mipi_set_data_hs_mode();
#endif
	if (power_mode == 0x9c) {
		pr_debug("otm8019a_check_esd OK!\n");
		return 1;
	} else {
		printk(KERN_ERR "otm8019a_check_esd fail!(0x%x)\n", power_mode);
		return 0;
	}
}

static int32_t otm8019a_enter_sleep(struct panel_spec *self, uint8_t is_sleep)
{
	int32_t i;
	LCM_Init_Code *sleep_in_out = NULL;
	unsigned int tag;
	int32_t size = 0;

	mipi_gen_write_t mipi_gen_write = self->info.mipi->ops->mipi_gen_write;
	mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;

	printk(KERN_DEBUG "otm8019a_enter_sleep, is_sleep = %d\n", is_sleep);

	if (is_sleep) {
		sleep_in_out = sleep_in;
		size = ARRAY_SIZE(sleep_in);
	} else {
		sleep_in_out = sleep_out;
		size = ARRAY_SIZE(sleep_out);
	}
	mipi_eotp_set(1, 0);

	for (i = 0; i < size; i++) {
		tag = (sleep_in_out->tag >> 24);
		if (tag & LCM_TAG_SEND)
			mipi_gen_write(sleep_in_out->data, (sleep_in_out->tag & LCM_TAG_MASK));
		else if (tag & LCM_TAG_SLEEP)
			msleep((sleep_in_out->tag & LCM_TAG_MASK));
		sleep_in_out++;
	}
	mipi_eotp_set(1, 1);

	return 0;
}

static struct panel_operations lcd_otm8019a_mipi_operations = {
	.panel_init = otm8019a_mipi_init,
	.panel_readid = otm8019a_readid,
	.panel_esd_check = otm8019a_check_esd,
	.panel_enter_sleep = otm8019a_enter_sleep,
};

static struct timing_rgb lcd_otm8019a_mipi_timing = {
	.hfp = 80,  /* unit: pixel */
	.hbp = 80,
	.hsync = 6,
	.vfp = 10, /*unit: line*/
	.vbp = 10,
	.vsync = 6,
};

static struct info_mipi lcd_otm8019a_mipi_info = {
	.work_mode				= SPRDFB_MIPI_MODE_VIDEO,
	.video_bus_width		= 24, /*18,16*/
	.lan_number			= 2,
	.phy_feq				= 500*1000,
	.h_sync_pol				= SPRDFB_POLARITY_POS,
	.v_sync_pol				= SPRDFB_POLARITY_POS,
	.de_pol					= SPRDFB_POLARITY_POS,
	.te_pol					= SPRDFB_POLARITY_POS,
	.color_mode_pol			= SPRDFB_POLARITY_NEG,
	.shut_down_pol			= SPRDFB_POLARITY_NEG,
	.timing					= &lcd_otm8019a_mipi_timing,
	.ops					= NULL,
};

struct panel_spec lcd_otm8019a_mipi_spec = {
	.width					= 480,
	.height					= 854,
	.fps					= 60,
	.reset_timing = {5, 40, 20},
	.type					= LCD_MODE_DSI,
	.direction				= LCD_DIRECT_NORMAL,
	.is_clean_lcd = true,
	/*.suspend_mode = SEND_SLEEP_CMD,*/
	.info = {
		.mipi				= &lcd_otm8019a_mipi_info
	},
	.ops					= &lcd_otm8019a_mipi_operations,
};

struct panel_cfg lcd_otm8019a_mipi = {
	/* this panel can only be main lcd */
	.dev_id = SPRDFB_MAINLCD_ID,
	.lcd_id = 0x8019,
	.lcd_name = "lcd_otm8019a_mipi",
	.panel = &lcd_otm8019a_mipi_spec,
};

static int __init lcd_otm8019a_mipi_init(void)
{
	return sprdfb_panel_register(&lcd_otm8019a_mipi);
}

subsys_initcall(lcd_otm8019a_mipi_init);

