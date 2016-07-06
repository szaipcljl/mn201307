
/* drivers/video/sc8825/lcd_st7701_mipi.c
 *
 * Support for st7701 mipi LCD device
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

//#include <asm/arch/sprd_lcd.h>
#include "../sprdfb_chip_common.h"
#include "../sprdfb_panel.h"
#include "../sprdfb.h"

#define printk printf

#define  LCD_DEBUG
#ifdef LCD_DEBUG
#define LCD_PRINT printk
#else
#define LCD_PRINT(...)
#endif

#define MAX_DATA   64

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
#if 0 //s66,TN屏
//--------------------------------------ST7701 Reset Sequence---------------------------------------//
//LCD_Nreset=1;
//Delayms (1); //Delay 1ms
//LCD_Nreset=0;
//Delayms (1); //Delay 1ms
//LCD_Nreset=1;
//Delayms (10);

{LCM_SEND(1),{0x01}},
LCM_SLEEP(120)},
{LCM_SEND(8),{6,0,0xFF,0x77,0x01,0x00,0x00,0x11}},
{LCM_SEND(2),{0xD1,0x11}},

{LCM_SEND(1),{0x11}},
//------------------------------------------Bank0 Setting----------------------------------------------------//
//------------------------------------Display Control setting----------------------------------------------//
{LCM_SLEEP(200)},
{LCM_SEND(8),{6,0,0xFF,0x77,0x01,0x00,0x00,0x10}},

{LCM_SEND(5),{3,0,0xC0,0xE9,0x03}},

{LCM_SEND(5),{3,0,0xC1,0x11,0x02}},

{LCM_SEND(5),{3,0,0xC2,0x31,0x08}},
//-------------------------------------Gamma Cluster Setting-------------------------------------------//
{LCM_SEND(19),{17,0,0xB0,0x00,0x06,0x11,0x12,0x18,0x0A,0x0A,0x09,0x09,0x1D,0x09,0x14,0x10,0x0E,0x11,0x19}},

{LCM_SEND(19),{17,0,0xB1,0x00,0x06,0x11,0x11,0x15,0x09,0x0B,0x09,0x09,0x23,0x09,0x17,0x14,0x18,0x1E,0x19}},
//---------------------------------------End Gamma Setting----------------------------------------------//
//------------------------------------End Display Control setting----------------------------------------//
//-----------------------------------------Bank0 Setting End---------------------------------------------//
//-------------------------------------------Bank1 Setting---------------------------------------------------//
//-------------------------------- Power Control Registers Initial --------------------------------------//
{LCM_SEND(8),{6,0,0xFF,0x77,0x01,0x00,0x00,0x11}},

{LCM_SEND(2),{0xB0,0x4D}},
//-------------------------------------------Vcom Setting---------------------------------------------------//
{LCM_SEND(2),{0xB1,0x4F}},
//-----------------------------------------End Vcom Setting-----------------------------------------------//
{LCM_SEND(2),{0xB2,0x07}},

{LCM_SEND(2),{0xB3,0x80}},

{LCM_SEND(2),{0xB5,0x47}},

{LCM_SEND(2),{0xB7,0x8A}},

{LCM_SEND(2),{0xB8,0x21}},

{LCM_SEND(2),{0xC1,0x78}},

{LCM_SEND(2),{0xC2,0x78}},

{LCM_SEND(2),{0xD0,0x88}},
//---------------------------------End Power Control Registers Initial -------------------------------//
{LCM_SLEEP(100)},
//---------------------------------------------GIP Setting----------------------------------------------------//
{LCM_SEND(6),{4,0,0xE0,0x00,0x00,0x02}},

{LCM_SEND(14),{12,0,0xE1,0x08,0x00,0x0A,0x00,0x07,0x00,0x09,0x00,0x00,0x33,0x33}},

{LCM_SEND(16),{14,0,0xE2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

{LCM_SEND(7),{5,0,0xE3,0x00,0x00,0x33,0x33}},

{LCM_SEND(5),{3,0,0xE4,0x44,0x44}},

{LCM_SEND(19),{17,0,0xE5,0x0E,0x60,0xAF,0xAF,0x10,0x60,0xAF,0xAF,0x0A,0x60,0xAF,0xAF,0x0C,0x60,0xAF,0xAF}},

{LCM_SEND(7),{5,0,0xE6,0x00,0x00,0x33,0x33}},

{LCM_SEND(5),{3,0,0xE7,0x44,0x44}},

{LCM_SEND(19),{17,0,0xE8,0x0D,0x60,0xAF,0xAF,0x0F,0x60,0xAF,0xAF,0x09,0x60,0xAF,0xAF,0x0B,0x60,0xAF,0xAF}},

{LCM_SEND(10),{8,0,0xEB,0x02,0x01,0xE4,0xE4,0x44,0x00,0x40}},

{LCM_SEND(5),{3,0,0xEC,0x02,0x01}},

{LCM_SEND(19),{17,0,0xED,0xAB,0x89,0x76,0x54,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x10,0x45,0x67,0x98,0xBA}},
//---------------------------------------------End GIP Setting-----------------------------------------------//
//------------------------------ Power Control Registers Initial End-----------------------------------//
//------------------------------------------Bank1 Setting----------------------------------------------------//
{LCM_SEND(8),{6,0,0xFF,0x77,0x01,0x00,0x00,0x00}},
{LCM_SLEEP(10)},
{LCM_SEND(1),{0x29}},
#else //Q01, IPS屏

//--------------------------------------ST7701 Reset Sequence---------------------------------------//
//LCD_Nreset=1;
//Delayms (1); //Delay 1ms
//LCD_Nreset=0;
//Delayms (1); //Delay 1ms
//LCD_Nreset=1;
//Delayms (10);

{LCM_SEND(1),{0x11}},
//------------------------------------------Bank0 Setting----------------------------------------------------//
//------------------------------------Display Control setting----------------------------------------------//
{LCM_SLEEP(200)},
	
{LCM_SEND(2),{0x55,0x90}},

{LCM_SEND(8),{6,0,0xFF,0x77,0x01,0x00,0x00,0x10}},

{LCM_SEND(5),{3,0,0xC0,0xE9,0x03}},

{LCM_SEND(5),{3,0,0xC1,0x12,0x02}},

{LCM_SEND(5),{3,0,0xC2,0x31,0x08}},
//-------------------------------------Gamma Cluster Setting-------------------------------------------//
{LCM_SEND(19),{17,0,0xB0,0x00,0x12,0x19,0x0c,0x11,0x06,0x04,0x07,0x06,0x20,0x03,0x12,0x0f,0x2c,0x35,0x1f}},

{LCM_SEND(19),{17,0,0xB1,0x00,0x12,0x18,0x10,0x13,0x06,0x08,0x06,0x08,0x20,0x04,0x11,0x10,0x2d,0x34,0x1f}},
//---------------------------------------End Gamma Setting----------------------------------------------//
//------------------------------------End Display Control setting----------------------------------------//
//-----------------------------------------Bank0 Setting End---------------------------------------------//
//-------------------------------------------Bank1 Setting---------------------------------------------------//
//-------------------------------- Power Control Registers Initial --------------------------------------//
{LCM_SEND(8),{6,0,0xFF,0x77,0x01,0x00,0x00,0x11}},

{LCM_SEND(2),{0xB0,0x5E}},
//-------------------------------------------Vcom Setting---------------------------------------------------//
{LCM_SEND(2),{0xB1,0x1e}},
//-----------------------------------------End Vcom Setting-----------------------------------------------//
{LCM_SEND(2),{0xB2,0x07}},

{LCM_SEND(2),{0xB3,0x80}},

{LCM_SEND(2),{0xB5,0x47}},

/*{LCM_SEND(2),{0xB7,0x89}},*/

{LCM_SEND(2),{0xB8,0x21}},

{LCM_SEND(2),{0xC1,0x78}},

{LCM_SEND(2),{0xC2,0x78}},

{LCM_SEND(2),{0xD0,0x88}},
//---------------------------------End Power Control Registers Initial -------------------------------//
{LCM_SLEEP(100)},
//---------------------------------------------GIP Setting----------------------------------------------------//
{LCM_SEND(6),{4,0,0xE0,0x00,0x00,0x02}},

{LCM_SEND(14),{12,0,0xE1,0x0b,0x00,0x0d,0x00,0x0c,0x00,0x0e,0x00,0x00,0x44,0x44}},

{LCM_SEND(16),{14,0,0xE2,0x33,0x33,0x44,0x44,0x64,0x00,0x66,0x00,0x65,0x00,0x67,0x00,0x00}},

{LCM_SEND(7),{5,0,0xE3,0x00,0x00,0x33,0x33}},

{LCM_SEND(5),{3,0,0xE4,0x44,0x44}},

{LCM_SEND(19),{17,0,0xE5,0x0c,0x78,0xa0,0xA0,0x0e,0x78,0xA0,0xA0,0x10,0x78,0xA0,0xA0,0x12,0x78,0xA0,0xA0}},

{LCM_SEND(7),{5,0,0xE6,0x00,0x00,0x33,0x33}},

{LCM_SEND(5),{3,0,0xE7,0x44,0x44}},

{LCM_SEND(19),{17,0,0xE8,0x0D,0x78,0xA0,0xA0,0x0F,0x78,0xA0,0xA0,0x11,0x78,0xA0,0xA0,0x13,0x78,0xA0,0xA0}},

{LCM_SEND(10),{8,0,0xEB,0x02,0x00,0x39,0x39,0xee,0x44,0x00}},

/*{LCM_SEND(5),{3,0,0xEC,0x02,0x01}},*/

{LCM_SEND(19),{17,0,0xED,0xff,0xf1,0x04,0x56,0x72,0x3F,0xFF,0xFF,0xFF,0xFF,0xF3,0x27,0x65,0x40,0x1f,0xff}},
//---------------------------------------------End GIP Setting-----------------------------------------------//
//------------------------------ Power Control Registers Initial End-----------------------------------//
//------------------------------------------Bank1 Setting----------------------------------------------------//
{LCM_SLEEP(10)},
{LCM_SEND(8),{6,0,0xFF,0x77,0x01,0x00,0x00,0x00}},
{LCM_SLEEP(10)},
{LCM_SEND(1),{0x29}},
#endif

};

static LCM_Init_Code disp_on =  {LCM_SEND(1), {0x29}};

static LCM_Init_Code sleep_in =  {LCM_SEND(1), {0x10}};

static LCM_Init_Code sleep_out =  {LCM_SEND(1), {0x11}};

static int32_t st7701_mipi_init(struct panel_spec *self)
{
	int32_t i;
	LCM_Init_Code *init = init_data;
	unsigned int tag;

	mipi_set_cmd_mode_t mipi_set_cmd_mode = self->info.mipi->ops->mipi_set_cmd_mode;
	mipi_gen_write_t mipi_gen_write = self->info.mipi->ops->mipi_gen_write;

	printk("st7701_init\n");

	mipi_set_cmd_mode();

	for(i = 0; i < ARRAY_SIZE(init_data); i++){
		tag = (init->tag >>24);
		if(tag & LCM_TAG_SEND){
			mipi_gen_write(init->data, (init->tag & LCM_TAG_MASK));
		}else if(tag & LCM_TAG_SLEEP){
			mdelay((init->tag & LCM_TAG_MASK));
		}
		init++;
	}
	printk("st7701_init end\n");
	return 0;
}

static LCM_Force_Cmd_Code rd_prep_code[]={
        {0x39, {LCM_SEND(8), {6,0,0xFF,0x77,0x01,0x00,0x00,0x13}}},
        {0x37, {LCM_SEND(2), {0x3, 0}}},
};

static uint32_t st7701_readid(struct panel_spec *self)
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
        mipi_set_lp_mode_t mipi_set_lp_mode = self->info.mipi->ops->mipi_set_lp_mode;

	printk("****lcd_st7701_mipi read id!\n");	
	mipi_set_lp_mode();
	for(j = 0; j < 4; j++){
		rd_prepare = rd_prep_code;
		for(i = 0; i < ARRAY_SIZE(rd_prep_code); i++){
			tag = (rd_prepare->real_cmd_code.tag >> 24);
			if(tag & LCM_TAG_SEND){
				mipi_force_write(rd_prepare->datatype, rd_prepare->real_cmd_code.data, (rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
			}else if(tag & LCM_TAG_SLEEP){
				mdelay((rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
			}
			rd_prepare++;
		}
		#if 1
		read_rtn = mipi_force_read(0xe0,1,(uint8_t *)&read_data[0]);
		LCD_PRINT("lcd_st7701_mipi read id 0xe0 value is 0x%x!\n",read_data[0]);

		read_rtn = mipi_force_read(0xa1, 2,(uint8_t *)read_data);
		LCD_PRINT("lcd_st7701_mipi read id 0xa1 value is 0x%x, 0x%x, 0x%x!\n", read_data[0], read_data[1], read_data[2]);
	
		#endif
		if(((0xff == read_data[0])&&(0xff == read_data[1])&&(0xff == read_data[2])) || ((0x77 == read_data[0])&&(0x01 == read_data[1]))){
			printk("lcd_st7701_mipi read id success!\n");
			return 0x7701;
		}
	}
	return 0x0;
}

static struct panel_operations lcd_st7701_mipi_operations = {
	.panel_init = st7701_mipi_init,
	.panel_readid = st7701_readid,
};

static struct timing_rgb lcd_st7701_mipi_timing = {
	.hfp = 60,  /* 80unit: pixel */
	.hbp = 50, //80
	.hsync = 60,
	.vfp = 20, /*unit: line*/
	.vbp =  14,//
	.vsync = 6,
};

static struct info_mipi lcd_st7701_mipi_info = {
	.work_mode  = SPRDFB_MIPI_MODE_VIDEO,
	.video_bus_width = 24, /*18,16*/
	.lan_number = 2,
	.phy_feq = 490*1000,
	.h_sync_pol = SPRDFB_POLARITY_POS,
	.v_sync_pol = SPRDFB_POLARITY_POS,
	.de_pol = SPRDFB_POLARITY_POS,
	.te_pol = SPRDFB_POLARITY_POS,
	.color_mode_pol = SPRDFB_POLARITY_NEG,
	.shut_down_pol = SPRDFB_POLARITY_NEG,
	.timing = &lcd_st7701_mipi_timing,
	.ops = NULL,
};

struct panel_spec lcd_st7701_mipi_spec = {
	.width = 480,
	.height = 854,
	.fps = 60,
	.type = LCD_MODE_DSI,
	.direction = LCD_DIRECT_NORMAL,
	.info = {
		.mipi = &lcd_st7701_mipi_info
	},
	.ops = &lcd_st7701_mipi_operations,
};

