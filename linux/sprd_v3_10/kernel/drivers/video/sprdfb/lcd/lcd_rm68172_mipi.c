/* drivers/video/sc8825/lcd_rm68172_mipi.c
 *
 * Support for rm68172 mipi LCD device
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


//#define LCD_Delay(ms)  uDelay(ms*1000)

#define MAX_DATA   64  ////48

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

static LCM_Init_Code init_data[] = {

{LCM_SEND(8),{6,0,0XF0,0X55,0XAA,0X52,0X08,0X02}},
{LCM_SEND(5),{3,0,0XF6,0X60,0X40}},
{LCM_SEND(7),{5,0,0XFE,0X01,0X80,0X09,0X09}},
{LCM_SEND(8),{6,0,0XF0,0X55,0XAA,0X52,0X08,0X01}},
{LCM_SEND(2),{0XB0,0X0D}},
{LCM_SEND(2),{0XB1,0X0D}},
{LCM_SEND(2),{0XB2,0X00}},
{LCM_SEND(2),{0XB5,0X08}},
{LCM_SEND(2),{0XB6,0X54}},
{LCM_SEND(2),{0XB7,0X44}},
{LCM_SEND(2),{0XB8,0X24}},
{LCM_SEND(2),{0XB9,0X34}},
{LCM_SEND(2),{0XBA,0X14}},
{LCM_SEND(6),{4,0,0XBC,0X00,0X58,0X00}},
{LCM_SEND(6),{4,0,0XBD,0X00,0X58,0X00}},
{LCM_SEND(5),{3,0,0XBE,0X00,0X36}},
{LCM_SEND(55),{53,0,0XD1,0X00,0X00,0X00,0X51,0X00,0X7E,0X00,0X9A,0X00,0XB0,0X00,0XD2,0X00,0XEE,0X01,0X1A,0X01,0X3C,0X01,0X71,0X01,0X9C,0X01,0XDF,0X02,0X16,0X02,0X18,0X02,0X4B,0X02,0X80,0X02,0XA1,0X02,0XCD,0X02,0XEB,0X03,0X11,0X03,0X27,0X03,0X42,0X03,0X4F,0X03,0X5A,0X03,0X62,0X03,0XFF}},
{LCM_SEND(55),{53,0,0XD2,0X00,0X00,0X00,0X51,0X00,0X7E,0X00,0X9A,0X00,0XB0,0X00,0XD2,0X00,0XEE,0X01,0X1A,0X01,0X3C,0X01,0X71,0X01,0X9C,0X01,0XDF,0X02,0X16,0X02,0X18,0X02,0X4B,0X02,0X80,0X02,0XA1,0X02,0XCD,0X02,0XEB,0X03,0X11,0X03,0X27,0X03,0X42,0X03,0X4F,0X03,0X5A,0X03,0X62,0X03,0XFF}},
{LCM_SEND(55),{53,0,0XD3,0X00,0X00,0X00,0X51,0X00,0X7E,0X00,0X9A,0X00,0XB0,0X00,0XD2,0X00,0XEE,0X01,0X1A,0X01,0X3C,0X01,0X71,0X01,0X9C,0X01,0XDF,0X02,0X16,0X02,0X18,0X02,0X4B,0X02,0X80,0X02,0XA1,0X02,0XCD,0X02,0XEB,0X03,0X11,0X03,0X27,0X03,0X42,0X03,0X4F,0X03,0X5A,0X03,0X62,0X03,0XFF}},
{LCM_SEND(55),{53,0,0XD4,0X00,0X00,0X00,0X51,0X00,0X7E,0X00,0X9A,0X00,0XB0,0X00,0XD2,0X00,0XEE,0X01,0X1A,0X01,0X3C,0X01,0X71,0X01,0X9C,0X01,0XDF,0X02,0X16,0X02,0X18,0X02,0X4B,0X02,0X80,0X02,0XA1,0X02,0XCD,0X02,0XEB,0X03,0X11,0X03,0X27,0X03,0X42,0X03,0X4F,0X03,0X5A,0X03,0X62,0X03,0XFF}},
{LCM_SEND(55),{53,0,0XD5,0X00,0X00,0X00,0X51,0X00,0X7E,0X00,0X9A,0X00,0XB0,0X00,0XD2,0X00,0XEE,0X01,0X1A,0X01,0X3C,0X01,0X71,0X01,0X9C,0X01,0XDF,0X02,0X16,0X02,0X18,0X02,0X4B,0X02,0X80,0X02,0XA1,0X02,0XCD,0X02,0XEB,0X03,0X11,0X03,0X27,0X03,0X42,0X03,0X4F,0X03,0X5A,0X03,0X62,0X03,0XFF}},
{LCM_SEND(55),{53,0,0XD6,0X00,0X00,0X00,0X51,0X00,0X7E,0X00,0X9A,0X00,0XB0,0X00,0XD2,0X00,0XEE,0X01,0X1A,0X01,0X3C,0X01,0X71,0X01,0X9C,0X01,0XDF,0X02,0X16,0X02,0X18,0X02,0X4B,0X02,0X80,0X02,0XA1,0X02,0XCD,0X02,0XEB,0X03,0X11,0X03,0X27,0X03,0X42,0X03,0X4F,0X03,0X5A,0X03,0X62,0X03,0XFF}},


{LCM_SEND(8),{6,0,0XF0,0X55,0XAA,0X52,0X08,0X03}},
{LCM_SEND(10),{8,0,0XB0,0X03,0X15,0XFA,0X00,0X00,0X00,0X00}},
{LCM_SEND(12),{10,0,0XB2,0XFB,0XFC,0XFD,0XFE,0XF0,0X20,0X00,0X83,0X04}},
{LCM_SEND(9),{7,0,0XB3,0X5B,0X00,0XFB,0X21,0X23,0X0C}},
{LCM_SEND(12),{10,0,0XB4,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00}},
{LCM_SEND(14),{12,0,0XB5,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X44}},
{LCM_SEND(10),{8,0,0XB6,0X00,0X00,0X00,0X00,0X00,0X00,0X00}},
{LCM_SEND(11),{9,0,0XB7,0X00,0X00,0X20,0X20,0X20,0X20,0X00,0X00}},
{LCM_SEND(6),{4,0,0XB8,0X00,0X00,0X00}},
{LCM_SEND(2),{0XB9,0X82}},

{LCM_SEND(19),{17,0,0XBA,0X45,0X8A,0X04,0X5F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF5,0X41,0XB9,0X54}},
{LCM_SEND(19),{17,0,0XBB,0X54,0XB9,0X15,0X4F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF4,0X50,0X8A,0X45}},
{LCM_SEND(7),{5,0,0XBC,0XC7,0XFF,0XFF,0XE3}},
{LCM_SEND(7),{5,0,0XBD,0XC7,0XFF,0XFF,0XE3}},
{LCM_SEND(7),{5,0,0XC0,0X00,0X01,0XFA,0X00}},
{LCM_SEND(8),{6,0,0XF0,0X55,0XAA,0X52,0X08,0X00}},
{LCM_SEND(5),{3,0,0XB0,0X00,0X10}},
{LCM_SEND(2),{0XB1,0XFC}},
{LCM_SEND(2),{0XBA,0X01}},
{LCM_SEND(2),{0XB4,0X10}},
{LCM_SEND(2),{0XB5,0X6B}},
{LCM_SEND(2),{0XBC,0X00}},
{LCM_SEND(2),{0X35,0X00}},
{LCM_SEND(2),{0x11,0x00}},
{LCM_SLEEP(120)},
{LCM_SEND(2),{0x29,0x00}},
{LCM_SLEEP(100)},
};

static LCM_Init_Code sleep_in[] =  {
//{LCM_SEND(2), {0x01, 0x00}},
//{LCM_SLEEP(100)},
{LCM_SEND(1), {0x28}},
{LCM_SLEEP(100)},
{LCM_SEND(1), {0x10}},
{LCM_SLEEP(100)},
{LCM_SEND(2), {0x4f,0x01}},
{LCM_SLEEP(120)},
};

static LCM_Init_Code sleep_out[] =  {
{LCM_SEND(1), {0x11}},
{LCM_SLEEP(120)},
{LCM_SEND(1), {0x29}},
{LCM_SLEEP(10)},
};

static LCM_Force_Cmd_Code rd_prep_code[]={
	{0x39, {LCM_SEND(8), {0x6, 0, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x01}}},
	{0x37, {LCM_SEND(2), {0x3, 0}}},
};

static LCM_Force_Cmd_Code rd_prep_code_1[]={
	{0x37, {LCM_SEND(2), {0x1, 0}}},
};
static int32_t rm68172_mipi_init(struct panel_spec *self)
{
	int32_t i;
	LCM_Init_Code *init = init_data;
	unsigned int tag;

	mipi_set_cmd_mode_t mipi_set_cmd_mode = self->info.mipi->ops->mipi_set_cmd_mode;
	mipi_gen_write_t mipi_gen_write = self->info.mipi->ops->mipi_gen_write;

#if 0
    volatile unsigned long temp = 0;

    temp = __raw_readl(CTL_PIN_BASE + REG_PIN_LCD_RSTN);
    temp &= ~(0xF); // Clear slp_wpu_wpd and slp_ie/oe bits; [3:0]
    temp |= (0x1 << 3); // SLP_WPU;
    __raw_writel(temp, CTL_PIN_BASE + REG_PIN_LCD_RSTN);
#endif

	pr_debug(KERN_DEBUG "rm68172_mipi_init\n");

	// Fixme;
	self->info.mipi->ops->mipi_set_lp_mode();

	mipi_set_cmd_mode();

	for(i = 0; i < ARRAY_SIZE(init_data); i++){
		tag = (init->tag >>24);
		if(tag & LCM_TAG_SEND){
			mipi_gen_write(init->data, (init->tag & LCM_TAG_MASK));
			udelay(20);
		}else if(tag & LCM_TAG_SLEEP){
			msleep((init->tag & LCM_TAG_MASK));
		}
		init++;
	}

	return 0;
}

static uint32_t rm68172_readid(struct panel_spec *self)
{
	//return 0x17;
	/*Jessica TODO: need read id*/
	int32_t i = 0;
	int32_t j =0;
	LCM_Force_Cmd_Code * rd_prepare = rd_prep_code;
	uint8_t read_data[3] = {0};
	int32_t read_rtn = 0;
	unsigned int tag = 0;
	mipi_set_cmd_mode_t mipi_set_cmd_mode = self->info.mipi->ops->mipi_set_cmd_mode;
	mipi_force_write_t mipi_force_write = self->info.mipi->ops->mipi_force_write;
	mipi_force_read_t mipi_force_read = self->info.mipi->ops->mipi_force_read;
	mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;

	printk("lcd_rm68172_mipi read id!\n");
	//return 0x8172;	//debug

	mipi_set_cmd_mode();
	mipi_eotp_set(0,1);
	for(j = 0; j < 4; j++){
		rd_prepare = rd_prep_code;
		for(i = 0; i < ARRAY_SIZE(rd_prep_code); i++){
			tag = (rd_prepare->real_cmd_code.tag >> 24);
			if(tag & LCM_TAG_SEND){
				mipi_force_write(rd_prepare->datatype, rd_prepare->real_cmd_code.data, (rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
			}else if(tag & LCM_TAG_SLEEP){
				msleep((rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
			}
			rd_prepare++;
		}
		read_rtn = mipi_force_read(0xc5, 3,(uint8_t *)read_data);
		printk("lcd_rm68172_mipi read id 0xc5 value is 0x%x, 0x%x, 0x%x!\n", read_data[0], read_data[1], read_data[2]);

		if((0x72 == read_data[0])&&(0x81 == read_data[1])){
			printk("lcd_rm68172_mipi read id success!\n");
			mipi_eotp_set(1,1);
			return 0x8172;
		}
	}
	mipi_eotp_set(1,1);
	return 0x0;

}

static int32_t rm68172_enter_sleep(struct panel_spec *self, uint8_t is_sleep)
{
	int32_t i;
	LCM_Init_Code *sleep_in_out = NULL;
	unsigned int tag;
	int32_t size = 0;
	mipi_set_cmd_mode_t mipi_set_cmd_mode = self->info.mipi->ops->mipi_set_cmd_mode;
	mipi_gen_write_t mipi_gen_write = self->info.mipi->ops->mipi_gen_write;
	printk(KERN_DEBUG "rm68172_enter_sleep, is_sleep = %d\n", is_sleep);

	if(is_sleep){
		sleep_in_out = sleep_in;
		size = ARRAY_SIZE(sleep_in);
	}else{
		sleep_in_out = sleep_out;
		size = ARRAY_SIZE(sleep_out);
	}
	
	self->ops->panel_reset(self);
	
	// Fixme;
	self->info.mipi->ops->mipi_set_lp_mode();
	
	mipi_set_cmd_mode();
	for(i = 0; i <size ; i++){
		tag = (sleep_in_out->tag >>24);
		if(tag & LCM_TAG_SEND){
			mipi_gen_write(sleep_in_out->data, (sleep_in_out->tag & LCM_TAG_MASK));
			udelay(20);
		}else if(tag & LCM_TAG_SLEEP){
			msleep((sleep_in_out->tag & LCM_TAG_MASK));
		}
		sleep_in_out++;
	}
	
	// Fixme;
	self->info.mipi->ops->mipi_set_hs_mode();

	return 0;
}

static uint32_t rm68172_readpowermode(struct panel_spec *self)
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

	pr_debug("lcd_rm68172_mipi read power mode!\n");
	mipi_eotp_set(0,1);
	for(j = 0; j < 4; j++){
		rd_prepare = rd_prep_code_1;
		for(i = 0; i < ARRAY_SIZE(rd_prep_code_1); i++){
			tag = (rd_prepare->real_cmd_code.tag >> 24);
			if(tag & LCM_TAG_SEND){
				mipi_force_write(rd_prepare->datatype, rd_prepare->real_cmd_code.data, (rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
			}else if(tag & LCM_TAG_SLEEP){
				msleep((rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
			}
			rd_prepare++;
		}
		read_rtn = mipi_force_read(0x0A, 1,(uint8_t *)read_data);
		pr_debug("lcd_rm68172 mipi read power mode 0x0A value is 0x%x! , read result(%d)\n", read_data[0], read_rtn);
		if((0x9c == read_data[0])  && (0 == read_rtn)){
			pr_debug("lcd_rm68172_mipi read power mode success!\n");
			mipi_eotp_set(1,1);
			return 0x9c;
		}
	}

	printk("lcd_rm68172 mipi read power mode fail!0x0A value is 0x%x! , read result(%d)\n", read_data[0], read_rtn);
	mipi_eotp_set(1,1);
	return 0x0;
}

static int32_t rm68172_check_esd(struct panel_spec *self)
{
	uint32_t power_mode;

	mipi_set_lp_mode_t mipi_set_data_lp_mode = self->info.mipi->ops->mipi_set_data_lp_mode;
	mipi_set_hs_mode_t mipi_set_data_hs_mode = self->info.mipi->ops->mipi_set_data_hs_mode;
	mipi_set_lp_mode_t mipi_set_lp_mode = self->info.mipi->ops->mipi_set_lp_mode;
	mipi_set_hs_mode_t mipi_set_hs_mode = self->info.mipi->ops->mipi_set_hs_mode;
	uint16_t work_mode = self->info.mipi->work_mode;
/*
	pr_debug("rm68172_check_esd!\n");
	if(SPRDFB_MIPI_MODE_CMD==work_mode){
		mipi_set_lp_mode();
	}else{
		mipi_set_data_lp_mode();
	}*/
	power_mode = rm68172_readpowermode(self);
	//power_mode = 0x0;
	/*
	if(SPRDFB_MIPI_MODE_CMD==work_mode){
		mipi_set_hs_mode();
	}else{
		mipi_set_data_hs_mode();
	}*/
	if(power_mode == 0x9c){
		printk("rm68172_check_esd OK!\n");
		return 1;
	}else{
		printk("rm68172_check_esd fail!(0x%x)\n", power_mode);
		return 0;
	}
}

static int32_t rm68172_after_suspend(struct panel_spec *self)
{
    // Do nothing;

    return 0;
}

static struct panel_operations lcd_rm68172_mipi_operations = {
	.panel_init = rm68172_mipi_init,
	.panel_readid = rm68172_readid,
	.panel_enter_sleep = rm68172_enter_sleep,
	.panel_esd_check = rm68172_check_esd,
	.panel_after_suspend = rm68172_after_suspend,
};

static struct timing_rgb lcd_rm68172_mipi_timing = {
	.hfp = 24,  /* 80unit: pixel */
	.hbp = 16, //80
	.hsync = 8,
	.vfp = 18, /*unit: line*/
	.vbp = 10,//yu goa pipei   huaping  huo dianbuliang 
	.vsync = 2,
};

static struct info_mipi lcd_rm68172_mipi_info = {
	.work_mode  = SPRDFB_MIPI_MODE_VIDEO,
	.video_bus_width = 24, /*18,16*/
	.lan_number = 2,
	.phy_feq = 480*1000,
	.h_sync_pol = SPRDFB_POLARITY_POS,
	.v_sync_pol = SPRDFB_POLARITY_POS,

	.de_pol = SPRDFB_POLARITY_POS,
	.te_pol = SPRDFB_POLARITY_POS,
	.color_mode_pol = SPRDFB_POLARITY_NEG,
	.shut_down_pol = SPRDFB_POLARITY_NEG,
	.timing = &lcd_rm68172_mipi_timing,
	.ops = NULL,
};

struct panel_spec lcd_rm68172_mipi_spec = {
	.width = 480,
	.height = 854,
	.fps = 60,
	.type = LCD_MODE_DSI,
	.direction = LCD_DIRECT_NORMAL,
//	.is_clean_lcd = true,
	.info = {
		.mipi = &lcd_rm68172_mipi_info
	},
	.ops = &lcd_rm68172_mipi_operations,
	.suspend_mode = SEND_SLEEP_CMD, //唤醒闪白屏，可注释试一下
};

struct panel_cfg lcd_rm68172_mipi = {
	/* this panel can only be main lcd */
	.dev_id = SPRDFB_MAINLCD_ID,
	.lcd_id = 0x8172,
	.lcd_name = "lcd_rm68172_mipi",
	.panel = &lcd_rm68172_mipi_spec,
};

static int __init lcd_rm68172_mipi_init(void)
{
	return sprdfb_panel_register(&lcd_rm68172_mipi);
}

subsys_initcall(lcd_rm68172_mipi_init);
