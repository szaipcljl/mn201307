/* drivers/video/sc8825/lcd_ili9881c_mipi.c
 *
 * Support for ili9881c mipi LCD device
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

//#define  LCD_DEBUG
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

static LCM_Init_Code init_data[] = {

{LCM_SEND(6),{4,0,0XFF,98,81,03}},
//GIP_1
{LCM_SEND(2),{0X01,0X00}},
{LCM_SEND(2),{0X02,0X00}},
{LCM_SEND(2),{0X03,0X53}},
{LCM_SEND(2),{0X04,0X13}},
{LCM_SEND(2),{0X05,0X13}},
{LCM_SEND(2),{0X06,0X06}},
{LCM_SEND(2),{0X07,0X00}},
{LCM_SEND(2),{0X08,0X04}},
{LCM_SEND(2),{0X09,0X04}},
{LCM_SEND(2),{0X0a,0X03}},
{LCM_SEND(2),{0X0b,0X03}},
{LCM_SEND(2),{0X0c,0X00}},
{LCM_SEND(2),{0X0d,0X00}},
{LCM_SEND(2),{0X0e,0X00}},
{LCM_SEND(2),{0X0f,0X04}},
{LCM_SEND(2),{0X10,0X04}},
{LCM_SEND(2),{0X11,0X00}},
{LCM_SEND(2),{0X12,0X00}},
{LCM_SEND(2),{0X13,0X00}},
{LCM_SEND(2),{0X14,0X00}},
{LCM_SEND(2),{0X15,0X00}},
{LCM_SEND(2),{0X16,0X00}},
{LCM_SEND(2),{0X17,0X00}},
{LCM_SEND(2),{0X18,0X00}},
{LCM_SEND(2),{0X19,0X00}},
{LCM_SEND(2),{0X1a,0X00}},
{LCM_SEND(2),{0X1b,0X00}},
{LCM_SEND(2),{0X1c,0X00}},
{LCM_SEND(2),{0X1d,0X00}},
{LCM_SEND(2),{0X1e,0XC0}},
{LCM_SEND(2),{0X1f,0X80}},
{LCM_SEND(2),{0X20,0X04}},
{LCM_SEND(2),{0X21,0X0B}},
{LCM_SEND(2),{0X22,0X00}},
{LCM_SEND(2),{0X23,0X00}},
{LCM_SEND(2),{0X24,0X00}},
{LCM_SEND(2),{0X25,0X00}},
{LCM_SEND(2),{0X26,0X00}},
{LCM_SEND(2),{0X27,0X00}},
{LCM_SEND(2),{0X28,0X55}},
{LCM_SEND(2),{0X29,0X03}},
{LCM_SEND(2),{0X2A,0X00}},
{LCM_SEND(2),{0X2B,0X00}},
{LCM_SEND(2),{0X2C,0X00}},
{LCM_SEND(2),{0X2D,0X00}},
{LCM_SEND(2),{0X2E,0X00}},
{LCM_SEND(2),{0X2F,0X00}},
{LCM_SEND(2),{0X30,0X00}},
{LCM_SEND(2),{0X31,0X00}},
{LCM_SEND(2),{0X32,0X00}},
{LCM_SEND(2),{0X33,0X00}},
{LCM_SEND(2),{0X34,0X04}},
{LCM_SEND(2),{0X35,0X00}},
{LCM_SEND(2),{0X36,0X05}},
{LCM_SEND(2),{0X37,0X00}},
{LCM_SEND(2),{0X38,0X3c}},
{LCM_SEND(2),{0X39,0X00}},
{LCM_SEND(2),{0X3a,0X40}},
{LCM_SEND(2),{0X3b,0X40}},
{LCM_SEND(2),{0X3c,0X00}},
{LCM_SEND(2),{0X3d,0X00}},
{LCM_SEND(2),{0X3e,0X00}},
{LCM_SEND(2),{0X3f,0X00}},
{LCM_SEND(2),{0X40,0X00}},
{LCM_SEND(2),{0X41,0X00}},
{LCM_SEND(2),{0X42,0X00}},
{LCM_SEND(2),{0X43,0X00}},
{LCM_SEND(2),{0X44,0X00}},
//GIP_2
{LCM_SEND(2),{0X50,0X01}},
{LCM_SEND(2),{0X51,0X23}},
{LCM_SEND(2),{0X52,0X45}},
{LCM_SEND(2),{0X53,0X67}},
{LCM_SEND(2),{0X54,0X89}},
{LCM_SEND(2),{0X55,0XAB}},
{LCM_SEND(2),{0X56,0X01}},
{LCM_SEND(2),{0X57,0X23}},
{LCM_SEND(2),{0X58,0X45}},
{LCM_SEND(2),{0X59,0X67}},
{LCM_SEND(2),{0X5a,0X89}},
{LCM_SEND(2),{0X5b,0XAB}},
{LCM_SEND(2),{0X5c,0XCD}},
{LCM_SEND(2),{0X5d,0XEF}},
{LCM_SEND(2),{0X5e,0X01}},
{LCM_SEND(2),{0X5f,0X14}},
{LCM_SEND(2),{0X60,0X15}},
{LCM_SEND(2),{0X61,0X0C}},
{LCM_SEND(2),{0X62,0X0D}},
{LCM_SEND(2),{0X63,0X0E}},
{LCM_SEND(2),{0X64,0X0F}},
{LCM_SEND(2),{0X65,0X10}},
{LCM_SEND(2),{0X66,0X11}},
{LCM_SEND(2),{0X67,0X08}},
{LCM_SEND(2),{0X68,0X02}},
{LCM_SEND(2),{0X69,0X0A}},
{LCM_SEND(2),{0X6a,0X02}},
{LCM_SEND(2),{0X6b,0X02}},
{LCM_SEND(2),{0X6c,0X02}},
{LCM_SEND(2),{0X6d,0X02}},
{LCM_SEND(2),{0X6e,0X02}},
{LCM_SEND(2),{0X6f,0X02}},
{LCM_SEND(2),{0X70,0X02}},
{LCM_SEND(2),{0X71,0X02}},
{LCM_SEND(2),{0X72,0X06}},
{LCM_SEND(2),{0X73,0X02}},
{LCM_SEND(2),{0X74,0X02}},
{LCM_SEND(2),{0X75,0X14}},
{LCM_SEND(2),{0X76,0X15}},
{LCM_SEND(2),{0X77,0X11}},
{LCM_SEND(2),{0X78,0X10}},
{LCM_SEND(2),{0X79,0X0F}},
{LCM_SEND(2),{0X7a,0X0E}},
{LCM_SEND(2),{0X7b,0X0D}},
{LCM_SEND(2),{0X7c,0X0C}},
{LCM_SEND(2),{0X7d,0X06}},
{LCM_SEND(2),{0X7e,0X02}},
{LCM_SEND(2),{0X7f,0X0a}},
{LCM_SEND(2),{0X80,0X02}},
{LCM_SEND(2),{0X81,0X02}},
{LCM_SEND(2),{0X82,0X02}},
{LCM_SEND(2),{0X83,0X02}},
{LCM_SEND(2),{0X84,0X02}},
{LCM_SEND(2),{0X85,0X02}},
{LCM_SEND(2),{0X86,0X02}},
{LCM_SEND(2),{0X87,0X02}},
{LCM_SEND(2),{0X88,0X08}},
{LCM_SEND(2),{0X89,0X02}},
{LCM_SEND(2),{0X8a,0X02}},

//CMD_Page 8
{LCM_SEND(6),{4,0,0XFF,0X98,0x81,0x04}},
{LCM_SEND(2),{0X6c,0x15}},
//{LCM_SEND(2),{0X7a,0x10}},    //kaiqi vreg1\vreg2   
{LCM_SEND(2),{0X6e,0X3b}},            
{LCM_SEND(2),{0X6f,0X53}},            
{LCM_SEND(2),{0X3a,0Xa4}},            
{LCM_SEND(2),{0X8d,0X15}},           
{LCM_SEND(2),{0X87,0Xba}}, 
{LCM_SEND(2),{0Xb2,0Xd1}},           
{LCM_SEND(2),{0X26,0X76}},
//{LCM_SEND(2),{0X88,0X8f}},
//{LCM_SEND(2),{0X8B,0XE0}},
//CMD_Page 1
{LCM_SEND(6),{4,0,0XFF,0X98,0X81,0X01}},
{LCM_SEND(2),{0X22,0X0A}},               
{LCM_SEND(2),{0X31,0X00}}, 
{LCM_SEND(2),{0X53,0X7D}},               
{LCM_SEND(2),{0X55,0X88}},               
{LCM_SEND(2),{0X50,0Xa6}},               
{LCM_SEND(2),{0X51,0XA6}}, 
//{LCM_SEND(2),{0X58,0X80}},             
{LCM_SEND(2),{0X60,0X14}},               //SDT
{LCM_SEND(2),{0XA0,0X08}},               //VP255 Gamma P
{LCM_SEND(2),{0XA1,0X1F}},               //VP251
{LCM_SEND(2),{0XA2,0X2D}},               //VP247
{LCM_SEND(2),{0XA3,0X12}},               //VP243
{LCM_SEND(2),{0XA4,0X16}},               //VP239
{LCM_SEND(2),{0XA5,0X2A}},               //VP231
{LCM_SEND(2),{0XA6,0X1e}},               //VP219
{LCM_SEND(2),{0XA7,0X1f}},               //VP203
{LCM_SEND(2),{0XA8,0X8A}},               //VP175
{LCM_SEND(2),{0XA9,0X1B}},               //VP144
{LCM_SEND(2),{0XAA,0X28}},               //VP111
{LCM_SEND(2),{0XAB,0X74}},               //VP80
{LCM_SEND(2),{0XAC,0X1B}},               //VP52
{LCM_SEND(2),{0XAD,0X1a}},               //VP36
{LCM_SEND(2),{0XAE,0X4d}},               //VP24
{LCM_SEND(2),{0XAF,0X22}},               //VP16
{LCM_SEND(2),{0XB0,0X28}},               //VP12
{LCM_SEND(2),{0XB1,0X4A}},               //VP8
{LCM_SEND(2),{0XB2,0X59}},               //VP4
{LCM_SEND(2),{0XB3,0X2C}},               //VP0

{LCM_SEND(2),{0XC0,0X08}},               //VN255 GAMMA N
{LCM_SEND(2),{0XC1,0X1F}},               //VN251
{LCM_SEND(2),{0XC2,0X2D}},               //VN247
{LCM_SEND(2),{0XC3,0X12}},               //VN243
{LCM_SEND(2),{0XC4,0X16}},               //VN239
{LCM_SEND(2),{0XC5,0X2A}},               //VN231
{LCM_SEND(2),{0XC6,0X1E}},               //VN219
{LCM_SEND(2),{0XC7,0X1f}},               //VN203
{LCM_SEND(2),{0XC8,0X8A}},               //VN175
{LCM_SEND(2),{0XC9,0X1B}},               //VN144
{LCM_SEND(2),{0XCA,0X28}},               //VN111
{LCM_SEND(2),{0XCB,0X74}},               //VN80
{LCM_SEND(2),{0XCC,0X1A}},               //VN52
{LCM_SEND(2),{0XCD,0X19}},               //VN36
{LCM_SEND(2),{0XCE,0X4d}},               //VN24
{LCM_SEND(2),{0XCF,0X22}},               //VN16
{LCM_SEND(2),{0XD0,0X28}},               //VN12
{LCM_SEND(2),{0XD1,0X4A}},               //VN8
{LCM_SEND(2),{0XD2,0X59}},               //VN4
{LCM_SEND(2),{0XD3,0X2C}},               //VN0


//CMD_Page 0
{LCM_SEND(6),{4,0,0XFF,0X98,0X81,0X00}},
{LCM_SEND(2),{0X11,0x00}},  //sleep out
{LCM_SLEEP(120)},
{LCM_SEND(2),{0X29,0x00}},  //display on
{LCM_SLEEP(20)},
};

static LCM_Init_Code disp_on =  {LCM_SEND(1), {0x29}};

static LCM_Init_Code sleep_in[] =  {
    {LCM_SEND(6),{4,0,0xFF,0x98,0x81,0x00}},
    {LCM_SLEEP(1)},

    {LCM_SEND(1), {0x28}},
    {LCM_SLEEP(120)},
    {LCM_SEND(1), {0x10}},
    {LCM_SLEEP(10)},
    {LCM_SEND(6),{4,0,0xFF,0x98,0x81,0x01}},
    {LCM_SLEEP(1)},

    {LCM_SEND(2), {0x58,0x91}},
};

static LCM_Init_Code sleep_out[] =  {
    {LCM_SEND(1), {0x11}},
    {LCM_SLEEP(120)},
    {LCM_SEND(1), {0x29}},
    {LCM_SLEEP(20)},
};

static LCM_Force_Cmd_Code rd_prep_code[]={
	{0x39, {LCM_SEND(6),{4,0,0xFF,0x98,0x81,0x01}}},
	{0x37, {LCM_SEND(2), {0x3, 0}}},
};

static LCM_Force_Cmd_Code rd_prep_code_1[]={
	{0x37, {LCM_SEND(2), {0x1, 0}}},
};
static int32_t ili9881c_mipi_init(struct panel_spec *self)
{
	int32_t i;
	LCM_Init_Code *init = init_data;
	unsigned int tag;

	mipi_set_cmd_mode_t mipi_set_cmd_mode = self->info.mipi->ops->mipi_set_cmd_mode;
	mipi_gen_write_t mipi_gen_write = self->info.mipi->ops->mipi_gen_write;
	mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;

	pr_debug(KERN_DEBUG "ili9881c_mipi_init\n");

	msleep(2);
	mipi_set_cmd_mode();
	mipi_eotp_set(1,0);

	for(i = 0; i < ARRAY_SIZE(init_data); i++){
		tag = (init->tag >>24);
		if(tag & LCM_TAG_SEND){
			mipi_gen_write(init->data, (init->tag & LCM_TAG_MASK));
			udelay(20);
		}else if(tag & LCM_TAG_SLEEP){
			//udelay((init->tag & LCM_TAG_MASK) * 1000);
				msleep(init->tag & LCM_TAG_MASK);
		}
		init++;
	}
	mipi_eotp_set(1,1);

	return 0;
}

static uint32_t ili9881c_readid(struct panel_spec *self)
{
	/*Jessica TODO: need read id*/
	int32_t i = 0;
	uint32_t j =0;
	LCM_Force_Cmd_Code * rd_prepare = rd_prep_code;
	uint8_t read_data[3] = {0};
	int32_t read_rtn = 0;
	unsigned int tag = 0;
	mipi_set_cmd_mode_t mipi_set_cmd_mode = self->info.mipi->ops->mipi_set_cmd_mode;
	mipi_force_write_t mipi_force_write = self->info.mipi->ops->mipi_force_write;
	mipi_force_read_t mipi_force_read = self->info.mipi->ops->mipi_force_read;
	mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;

	printk("lcd_ili9881c_mipi read id!\n");

	mipi_set_cmd_mode();
	mipi_eotp_set(1,0);

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
		mdelay(50);

		read_rtn = mipi_force_read(0x00, 1,(uint8_t *)&read_data[0]);//huafeizhoyu141030 add
		read_rtn = mipi_force_read(0x01, 1,(uint8_t *)&read_data[1]);//huafeizhoyu141030 add
		read_rtn = mipi_force_read(0x02, 1,(uint8_t *)&read_data[2]);//huafeizhoyu141030 mod
		printk("lcd_ili9881c_mipi read id value is 0x%x,0x%x,0x%x!\n", read_data[0],read_data[1],read_data[2]);//



		//if((0x98 == read_data[0]) && (0x81 == read_data[1]) && (0x0C == read_data[2])){
		if((0x81 == read_data[0]) && (0x98 == read_data[1])){ //zhangbei 160107 add
			printk("lcd_ili9881c_mipi read id success!\n");
			mipi_eotp_set(1,1);
			return 0x9881;
		}
	}
	mipi_eotp_set(1,1);
	return 0x0;
}

static int32_t ili9881c_enter_sleep(struct panel_spec *self, uint8_t is_sleep)
{
	int32_t i;
	LCM_Init_Code *sleep_in_out = NULL;
	unsigned int tag;
	int32_t size = 0;

	mipi_gen_write_t mipi_gen_write = self->info.mipi->ops->mipi_gen_write;
	mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;

	printk(KERN_DEBUG "ili9881c_enter_sleep, is_sleep = %d\n", is_sleep);

	if(is_sleep){
		sleep_in_out = sleep_in;
		size = ARRAY_SIZE(sleep_in);
	}else{
		sleep_in_out = sleep_out;
		size = ARRAY_SIZE(sleep_out);
	}
	mipi_eotp_set(1,0);

	for(i = 0; i <size ; i++){
		tag = (sleep_in_out->tag >>24);
		if(tag & LCM_TAG_SEND){
			mipi_gen_write(sleep_in_out->data, (sleep_in_out->tag & LCM_TAG_MASK));
		}else if(tag & LCM_TAG_SLEEP){
			//udelay((sleep_in_out->tag & LCM_TAG_MASK) * 1000);
			msleep((sleep_in_out->tag & LCM_TAG_MASK));
		}
		sleep_in_out++;
	}
	return 0;
}

static uint32_t ili9881c_readpowermode(struct panel_spec *self)
{
	int32_t i = 0;
	uint32_t j =0;
	LCM_Force_Cmd_Code * rd_prepare = rd_prep_code_1;
	uint8_t read_data[1] = {0};
	int32_t read_rtn = 0;
	unsigned int tag = 0;
	uint32_t reg_val_1 = 0;
	uint32_t reg_val_2 = 0;

	mipi_force_write_t mipi_force_write = self->info.mipi->ops->mipi_force_write;
	mipi_force_read_t mipi_force_read = self->info.mipi->ops->mipi_force_read;
	mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;

	pr_debug("lcd_ili9881c_mipi read power mode!\n");
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
		//printk("lcd_ili9881c mipi read power mode 0x0A value is 0x%x! , read result(%d)\n", read_data[0], read_rtn);

		if((0x9c == read_data[0])  && (0 == read_rtn)){
			pr_debug("lcd_ili9881c_mipi read power mode success!\n");
			mipi_eotp_set(1,1);
			return 0x9c;
		}
	}

	mipi_eotp_set(1,1);
	return 0x0;
}


static uint32_t ili9881c_check_esd(struct panel_spec *self)
{
	uint32_t power_mode;

	pr_debug("ili9881c_check_esd!\n");
	mipi_set_lp_mode_t mipi_set_data_lp_mode = self->info.mipi->ops->mipi_set_data_lp_mode;
	mipi_set_hs_mode_t mipi_set_data_hs_mode = self->info.mipi->ops->mipi_set_data_hs_mode;

	mipi_set_lp_mode_t mipi_set_lp_mode = self->info.mipi->ops->mipi_set_lp_mode;
	mipi_set_hs_mode_t mipi_set_hs_mode = self->info.mipi->ops->mipi_set_hs_mode;
	uint16_t work_mode = self->info.mipi->work_mode;

	if(SPRDFB_MIPI_MODE_CMD==work_mode){
		mipi_set_lp_mode();
	}else{
		mipi_set_data_lp_mode();
	}
	power_mode = ili9881c_readpowermode(self);
	//power_mode = 0x0;
	if(SPRDFB_MIPI_MODE_CMD==work_mode){
		mipi_set_hs_mode();
	}else{
		mipi_set_data_hs_mode();
	}
	if(power_mode == 0x9c){
		pr_debug("ili9881c_check_esd OK!\n");
		return 1;
	}else{
		printk("ili9881c_check_esd fail!(0x%x)\n", power_mode);
		return 0;
	}
}

static uint32_t ili9881c_mipi_after_suspend(struct panel_spec *self)
{
	return 0;
}

static struct panel_operations lcd_ili9881c_mipi_operations = {
	.panel_init = ili9881c_mipi_init,
	.panel_readid = ili9881c_readid,
	.panel_enter_sleep = ili9881c_enter_sleep,
	.panel_esd_check = ili9881c_check_esd,
	.panel_after_suspend = ili9881c_mipi_after_suspend,
};

static struct timing_rgb lcd_ili9881c_mipi_timing = {
	.hfp = 80,  /* unit: pixel */// 100
	.hbp = 100,//80
	.hsync = 40,//6
	.vfp = 10, /*unit: line*/
	.vbp = 20,
	.vsync =10, //6,
};

static struct info_mipi lcd_ili9881c_mipi_info = {
	.work_mode  = SPRDFB_MIPI_MODE_VIDEO,
	.video_bus_width = 24, /*18,16*/
	.lan_number = 4,
	.phy_feq = 400*1000,
	.h_sync_pol = SPRDFB_POLARITY_POS,
	.v_sync_pol = SPRDFB_POLARITY_POS,
	.de_pol = SPRDFB_POLARITY_POS,
	.te_pol = SPRDFB_POLARITY_POS,
	.color_mode_pol = SPRDFB_POLARITY_NEG,
	.shut_down_pol = SPRDFB_POLARITY_NEG,
	.timing = &lcd_ili9881c_mipi_timing,
	.ops = NULL,
};

struct panel_spec lcd_ili9881c_mipi_spec = {
	//.cap = PANEL_CAP_NOT_TEAR_SYNC,
	.width = 720,
	.height = 1280,
	.fps = 60,
	.type = LCD_MODE_DSI,
	.direction = LCD_DIRECT_NORMAL,
	.is_clean_lcd = true,
	.reset_timing = {5, 15, 120},
	.info = {
		.mipi = &lcd_ili9881c_mipi_info
	},
	.ops = &lcd_ili9881c_mipi_operations,
};

struct panel_cfg lcd_ili9881c_mipi = {
	/* this panel can only be main lcd */
	.dev_id = SPRDFB_MAINLCD_ID,
	.lcd_id = 0x9881,
	.lcd_name = "lcd_ili9881c_mipi",
	.panel = &lcd_ili9881c_mipi_spec,
};

static int __init lcd_ili9881c_mipi_init(void)
{
	return sprdfb_panel_register(&lcd_ili9881c_mipi);
}

subsys_initcall(lcd_ili9881c_mipi_init);
