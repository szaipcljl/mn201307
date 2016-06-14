/* drivers/video/sprdfb/lcd_jd9365_mipi.c
 *
 * Support for jd9365 mipi LCD device
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

#include "../sprdfb_chip_common.h"
#include "../sprdfb.h"
#include "../sprdfb_panel.h"

#define printk printf

#define  LCD_DEBUG
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
 

//JD9365 initial code
//Page0
{LCM_SEND(2),{0xE0,0x00}},

//--- PASSWORD  ----//
{LCM_SEND(2),{0xE1,0x93}},
{LCM_SEND(2),{0xE2,0x65}},
{LCM_SEND(2),{0xE3,0xF8}},

//--- Sequence Ctrl ----//
{LCM_SEND(2),{0x70,0x20}},
{LCM_SEND(2),{0x71,0x13}},
{LCM_SEND(2),{0x72,0x06}},
{LCM_SEND(2),{0x75,0x03}},


//--- Page1  ----//
{LCM_SEND(2),{0xE0,0x01}},

//Set VCOM
{LCM_SEND(2),{0x00,0x00}},
{LCM_SEND(2),{0x01,0xB0}},
{LCM_SEND(2),{0x03,0x00}},
{LCM_SEND(2),{0x04,0xB0}},
{LCM_SEND(2),{0x0C,0x64}},

//Set Gamma Power,1, VGMP,1,GMN,1,VGSP,1,VGSN
{LCM_SEND(2),{0x17,0x00}},
{LCM_SEND(2),{0x18,0xB7}},
{LCM_SEND(2),{0x19,0x00}},
{LCM_SEND(2),{0x1A,0x00}},
{LCM_SEND(2),{0x1B,0xB7}},
{LCM_SEND(2),{0x1C,0x00}},

//Set Gate Power
{LCM_SEND(2),{0x1F,0x7E}},
{LCM_SEND(2),{0x20,0x24}},
{LCM_SEND(2),{0x21,0x24}},
{LCM_SEND(2),{0x22,0x4E}},
{LCM_SEND(2),{0x24,0xF8}},


//SET RGBCYC
{LCM_SEND(2),{0x37,0x09}},	//SS=1,1,BGR=1
{LCM_SEND(2),{0x38,0x04}},	//JDT=100 column inversion
{LCM_SEND(2),{0x3D,0xFF}},
{LCM_SEND(2),{0x3E,0xFF}},
{LCM_SEND(2),{0x3F,0x7F}},

//Set TCON
{LCM_SEND(2),{0x40,0x04}},	//RSO 04h=720,1,05h=768,1,06h=800
{LCM_SEND(2),{0x41,0xA0}},	//LN=640->1280 line

//--- power voltage  ----//
{LCM_SEND(2),{0x55,0x01}},
{LCM_SEND(2),{0x56,0x01}},
{LCM_SEND(2),{0x57,0x69}},
{LCM_SEND(2),{0x58,0x0A}},
{LCM_SEND(2),{0x59,0x2A}},
{LCM_SEND(2),{0x5A,0x1B}},
{LCM_SEND(2),{0x5B,0x18}},

//--- Gamma  ----//   //2.7
{LCM_SEND(2),{0x5D,0x68}},//0x68
{LCM_SEND(2),{0x5E,0x4D}},//0x4B
{LCM_SEND(2),{0x5F,0x3C}},//0x38
{LCM_SEND(2),{0x60,0x2D}},//0x2B
{LCM_SEND(2),{0x61,0x2A}},//0x26
{LCM_SEND(2),{0x62,0x1B}},//0x17
{LCM_SEND(2),{0x63,0x1F}},//0x1B
{LCM_SEND(2),{0x64,0x09}},//0x05
{LCM_SEND(2),{0x65,0x22}},//0x1E
{LCM_SEND(2),{0x66,0x21}},//0x1D
{LCM_SEND(2),{0x67,0x22}},//0x1E
{LCM_SEND(2),{0x68,0x43}},//0x3C
{LCM_SEND(2),{0x69,0x33}},//0x2E
{LCM_SEND(2),{0x6A,0x43}},//0x3B
{LCM_SEND(2),{0x6B,0x3B}},//0x2B
{LCM_SEND(2),{0x6C,0x3C}},//0x2C
{LCM_SEND(2),{0x6D,0x2A}},//0x22
{LCM_SEND(2),{0x6E,0x25}},//0x1D
{LCM_SEND(2),{0x6F,0x0F}},//0x0F
{LCM_SEND(2),{0x70,0x78}},//0x78
{LCM_SEND(2),{0x71,0x4D}},//0x4B
{LCM_SEND(2),{0x72,0x3C}},//0x38
{LCM_SEND(2),{0x73,0x2D}},//0x2B
{LCM_SEND(2),{0x74,0x2A}},//0x26
{LCM_SEND(2),{0x75,0x1B}},//0x17
{LCM_SEND(2),{0x76,0x1F}},//0x1B
{LCM_SEND(2),{0x77,0x09}},//0x05
{LCM_SEND(2),{0x78,0x22}},//0x1E
{LCM_SEND(2),{0x79,0x21}},//0x1D
{LCM_SEND(2),{0x7A,0x22}},//0x1E
{LCM_SEND(2),{0x7B,0x43}},//0x3C
{LCM_SEND(2),{0x7C,0x33}},//0x2E
{LCM_SEND(2),{0x7D,0x43}},//0x3B
{LCM_SEND(2),{0x7E,0x3B}},//0x2B
{LCM_SEND(2),{0x7F,0x3C}},//0x2C
{LCM_SEND(2),{0x80,0x2A}},//0x22
{LCM_SEND(2),{0x81,0x25}},//0x1D
{LCM_SEND(2),{0x82,0x0F}},//0x0F
                      

//Page2,1,for GIP
{LCM_SEND(2),{0xE0,0x02}},

//GIP_L Pin mapping
{LCM_SEND(2),{0x00,0x45}},
{LCM_SEND(2),{0x01,0x47}},
{LCM_SEND(2),{0x02,0x49}},
{LCM_SEND(2),{0x03,0x4B}},
{LCM_SEND(2),{0x04,0x41}},
{LCM_SEND(2),{0x05,0x43}},
{LCM_SEND(2),{0x06,0x1F}},
{LCM_SEND(2),{0x07,0x1F}},
{LCM_SEND(2),{0x08,0x1F}},
{LCM_SEND(2),{0x09,0x1F}},
{LCM_SEND(2),{0x0A,0x1F}},
{LCM_SEND(2),{0x0B,0x1F}},
{LCM_SEND(2),{0x0C,0x1F}},
{LCM_SEND(2),{0x0D,0x1F}},
{LCM_SEND(2),{0x0E,0x1F}},
{LCM_SEND(2),{0x0F,0x1E}},
{LCM_SEND(2),{0x10,0x1E}},
{LCM_SEND(2),{0x11,0x1E}},
{LCM_SEND(2),{0x12,0x1F}},
{LCM_SEND(2),{0x13,0x1F}},
{LCM_SEND(2),{0x14,0x1E}},
{LCM_SEND(2),{0x15,0x1F}},

//GIP_R Pin mapping
{LCM_SEND(2),{0x16,0x44}},
{LCM_SEND(2),{0x17,0x46}},
{LCM_SEND(2),{0x18,0x48}},
{LCM_SEND(2),{0x19,0x4A}},
{LCM_SEND(2),{0x1A,0x40}},
{LCM_SEND(2),{0x1B,0x42}},
{LCM_SEND(2),{0x1C,0x1F}},
{LCM_SEND(2),{0x1D,0x1F}},
{LCM_SEND(2),{0x1E,0x1F}},
{LCM_SEND(2),{0x1F,0x1F}},
{LCM_SEND(2),{0x20,0x1F}},
{LCM_SEND(2),{0x21,0x1F}},
{LCM_SEND(2),{0x22,0x1F}},
{LCM_SEND(2),{0x23,0x1F}},
{LCM_SEND(2),{0x24,0x1F}},
{LCM_SEND(2),{0x25,0x1E}},
{LCM_SEND(2),{0x26,0x1E}},
{LCM_SEND(2),{0x27,0x1E}},
{LCM_SEND(2),{0x28,0x1F}},
{LCM_SEND(2),{0x29,0x1F}},
{LCM_SEND(2),{0x2A,0x1E}},
{LCM_SEND(2),{0x2B,0x1F}},
                      
//GIP_L_GS Pin mapping
{LCM_SEND(2),{0x2C,0x0A}},
{LCM_SEND(2),{0x2D,0x08}},   
{LCM_SEND(2),{0x2E,0x06}}, 
{LCM_SEND(2),{0x2F,0x04}}, 
{LCM_SEND(2),{0x30,0x02}}, 
{LCM_SEND(2),{0x31,0x00}}, 
{LCM_SEND(2),{0x32,0x1F}}, 
{LCM_SEND(2),{0x33,0x1F}}, 
{LCM_SEND(2),{0x34,0x1F}}, 
{LCM_SEND(2),{0x35,0x1F}}, 
{LCM_SEND(2),{0x36,0x1F}}, 
{LCM_SEND(2),{0x37,0x1F}}, 
{LCM_SEND(2),{0x38,0x1F}}, 
{LCM_SEND(2),{0x39,0x1F}}, 
{LCM_SEND(2),{0x3A,0x1F}}, 
{LCM_SEND(2),{0x3B,0x1E}}, 
{LCM_SEND(2),{0x3C,0x1E}}, 
{LCM_SEND(2),{0x3D,0x1E}}, 
{LCM_SEND(2),{0x3E,0x1F}}, 
{LCM_SEND(2),{0x3F,0x1F}}, 
{LCM_SEND(2),{0x40,0x1F}}, 
{LCM_SEND(2),{0x41,0x1E}},
 
//GIP_R_GS Pin mapping
{LCM_SEND(2),{0x42,0x0B}},
{LCM_SEND(2),{0x43,0x09}},   
{LCM_SEND(2),{0x44,0x07}}, 
{LCM_SEND(2),{0x45,0x05}}, 
{LCM_SEND(2),{0x46,0x03}}, 
{LCM_SEND(2),{0x47,0x01}}, 
{LCM_SEND(2),{0x48,0x1F}}, 
{LCM_SEND(2),{0x49,0x1F}}, 
{LCM_SEND(2),{0x4A,0x1F}}, 
{LCM_SEND(2),{0x4B,0x1F}}, 
{LCM_SEND(2),{0x4C,0x1F}}, 
{LCM_SEND(2),{0x4D,0x1F}}, 
{LCM_SEND(2),{0x4E,0x1F}}, 
{LCM_SEND(2),{0x4F,0x1F}}, 
{LCM_SEND(2),{0x50,0x1F}}, 
{LCM_SEND(2),{0x51,0x1E}}, 
{LCM_SEND(2),{0x52,0x1E}}, 
{LCM_SEND(2),{0x53,0x1E}}, 
{LCM_SEND(2),{0x54,0x1F}}, 
{LCM_SEND(2),{0x55,0x1F}}, 
{LCM_SEND(2),{0x56,0x1F}}, 
{LCM_SEND(2),{0x57,0x1E}}, 

//GIP Timing  
{LCM_SEND(2),{0x58,0x40}}, 
{LCM_SEND(2),{0x59,0x00}}, 
{LCM_SEND(2),{0x5A,0x00}}, 
{LCM_SEND(2),{0x5B,0x30}}, 
{LCM_SEND(2),{0x5C,0x08}}, 
{LCM_SEND(2),{0x5D,0x40}}, 
{LCM_SEND(2),{0x5E,0x01}}, 
{LCM_SEND(2),{0x5F,0x02}}, 
{LCM_SEND(2),{0x60,0x00}}, 
{LCM_SEND(2),{0x61,0x01}}, 
{LCM_SEND(2),{0x62,0x02}}, 
{LCM_SEND(2),{0x63,0x69}}, 
{LCM_SEND(2),{0x64,0x6A}},
{LCM_SEND(2),{0x65,0x00}},
{LCM_SEND(2),{0x66,0x00}},
{LCM_SEND(2),{0x67,0x74}},
{LCM_SEND(2),{0x68,0x0A}},
{LCM_SEND(2),{0x69,0x69}},
{LCM_SEND(2),{0x6A,0x6A}},
{LCM_SEND(2),{0x6B,0x10}}, 
{LCM_SEND(2),{0x6C,0x00}}, 
{LCM_SEND(2),{0x6D,0x04}}, 
{LCM_SEND(2),{0x6E,0x04}}, 
{LCM_SEND(2),{0x6F,0x88}}, 
{LCM_SEND(2),{0x70,0x00}}, 
{LCM_SEND(2),{0x71,0x00}}, 
{LCM_SEND(2),{0x72,0x06}}, 
{LCM_SEND(2),{0x73,0x7B}}, 
{LCM_SEND(2),{0x74,0x00}}, 
{LCM_SEND(2),{0x75,0x87}}, 
{LCM_SEND(2),{0x76,0x00}}, 
{LCM_SEND(2),{0x77,0x5D}}, 
{LCM_SEND(2),{0x78,0x17}}, 
{LCM_SEND(2),{0x79,0x1F}}, 
{LCM_SEND(2),{0x7A,0x00}}, 
{LCM_SEND(2),{0x7B,0x00}}, 
{LCM_SEND(2),{0x7C,0x00}}, 
{LCM_SEND(2),{0x7D,0x03}}, 
{LCM_SEND(2),{0x7E,0x7B}}, 

{LCM_SEND(2),{0xE0,0x04}}, //ESD
{LCM_SEND(2),{0x2B,0x2B}},
{LCM_SEND(2),{0x2E,0x44}}, 
//Page0
{LCM_SEND(2),{0xE0,0x00}},
{LCM_SEND(2),{0xE6,0x02}},
{LCM_SEND(2),{0xE7,0x02}},
{LCM_SEND(2),{0x55,0x90}},//color

{LCM_SEND(2),{0x35,0x00}},//151124 TE

{LCM_SEND(1), {0x11}},                 // Sleep-Out

{LCM_SLEEP(120)},

{LCM_SEND(1), {0x29}},                 // Display On
{LCM_SLEEP(20)},

{LCM_SEND(2), {0xE0,0x00}},
{LCM_SEND(2), {0xE1,0x09}},
{LCM_SEND(2), {0xE2,0xB1}},
{LCM_SEND(2), {0xE3,0x7F}},



};

static LCM_Force_Cmd_Code rd_prep_code[]={
	//Page0
	{0x15,{LCM_SEND(2),{0xE0,0x00}}},

	//--- PASSWORD  ----//
	{0x15,{LCM_SEND(2),{0xE1,0x93}}},
	{0x15,{LCM_SEND(2),{0xE2,0x65}}},
	{0x15,{LCM_SEND(2),{0xE3,0xF8}}},

	{0x37, {LCM_SEND(2), {0x2, 0}}},
};

static LCM_Init_Code disp_on =  {LCM_SEND(1), {0x29}};

static LCM_Init_Code sleep_in =  {LCM_SEND(1), {0x10}};

static LCM_Init_Code sleep_out =  {LCM_SEND(1), {0x11}};

static int32_t jd9365_mipi_init(struct panel_spec *self)
{
	int32_t i;
	LCM_Init_Code *init = init_data;
	unsigned int tag;

	mipi_set_cmd_mode_t mipi_set_cmd_mode = self->info.mipi->ops->mipi_set_cmd_mode;
	mipi_gen_write_t mipi_gen_write = self->info.mipi->ops->mipi_gen_write;

         mipi_set_lp_mode_t mipi_set_lp_mode = self->info.mipi->ops->mipi_set_lp_mode;
         mipi_set_hs_mode_t mipi_set_hs_mode = self->info.mipi->ops->mipi_set_hs_mode;
	mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;
	printk("uboot lcd_jd9365_init\n");

	//mipi_set_cmd_mode();

	mdelay(5);
	mipi_set_lp_mode();
	//mipi_eotp_set(1,0);
	for(i = 0; i < ARRAY_SIZE(init_data); i++){
		tag = (init->tag >>24);
		if(tag & LCM_TAG_SEND){
			mipi_gen_write(init->data, (init->tag & LCM_TAG_MASK));
			udelay(20);
		}else if(tag & LCM_TAG_SLEEP){
			mdelay((init->tag & LCM_TAG_MASK));
		}
		init++;
	}

	mdelay(5);
	mipi_set_hs_mode();
	return 0;
}

static uint32_t jd9365_readid(struct panel_spec *self)
{
        /*Jessica TODO: need read id*/
        int32_t i = 0;
        uint32 j =0;
        LCM_Force_Cmd_Code * rd_prepare = rd_prep_code;
        uint8_t read_data[3] = {0};
        int32_t read_rtn = 0;
        unsigned int tag = 0;
        mipi_set_cmd_mode_t mipi_set_cmd_mode = self->info.mipi->ops->mipi_set_cmd_mode;
        mipi_force_write_t mipi_force_write = self->info.mipi->ops->mipi_force_write;
        mipi_force_read_t mipi_force_read = self->info.mipi->ops->mipi_force_read;
        mipi_set_lp_mode_t mipi_set_lp_mode = self->info.mipi->ops->mipi_set_lp_mode;
        mipi_set_hs_mode_t mipi_set_hs_mode = self->info.mipi->ops->mipi_set_hs_mode;
        mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;

	//return 0x6; //cg liyun 20130329

	printk("uboot lcd_jd9365_mipi read id!\n");
	#if 1
	mipi_set_lp_mode();

	//mipi_set_cmd_mode();
	for(j = 0; j < 4; j++){
		rd_prepare = rd_prep_code;
		for(i = 0; i < ARRAY_SIZE(rd_prep_code); i++){
			tag = (rd_prepare->real_cmd_code.tag >> 24);
			if(tag & LCM_TAG_SEND){
				mipi_force_write(rd_prepare->datatype, rd_prepare->real_cmd_code.data, (rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
				udelay(20);
			}else if(tag & LCM_TAG_SLEEP){
				mdelay((rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
			}
			rd_prepare++;
		}
		mdelay(50);

		read_rtn = mipi_force_read(0xda,1,&read_data[0]);
		printk("lcd_jd9365_mipi read id 0xda value is 0x%x!\n",read_data[0]);

		read_rtn = mipi_force_read(0xdb,1,&read_data[1]);
		printk("lcd_jd9365_mipi read id 0xdb value is 0x%x!\n",read_data[1]);

		read_rtn = mipi_force_read(0xdc,1,&read_data[2]);
		printk("lcd_jd9365_mipi read id 0xdc value is 0x%x!\n",read_data[2]);		
		//read_rtn = mipi_force_read(0x04, 1,(uint8_t *)read_data);
		//printk("lcd_jd9365_mipi read id 0x04 value is 0x%x!\n", read_data[0]);

		if((0x93 == read_data[0])&&(0x65 == read_data[1])){
			printk("lcd_jd9365_mipi read id success!\n");
			return 0x9365;
		}
	}

	mdelay(5);
	mipi_set_hs_mode();
	#endif
	return 0;
}

static struct panel_operations lcd_jd9365_mipi_operations = {
	.panel_init = jd9365_mipi_init,
	.panel_readid = jd9365_readid,
};

static struct timing_rgb lcd_jd9365_mipi_timing = {
	.hfp = 50,  /* unit: pixel */// 100//151125: 20
	.hbp = 50,//80//151125 20
	.hsync = 20,//6
	.vfp = 8, /*unit: line*/
	.vbp = 16,
	.vsync =2, //6,
};

static struct info_mipi lcd_jd9365_mipi_info = {
	.work_mode  = SPRDFB_MIPI_MODE_VIDEO,
	.video_bus_width = 24, /*18,16*/
	.lan_number = 4,
	.phy_feq = 460*1000, //500->10MHz,400->8M in lp mode
	.h_sync_pol = SPRDFB_POLARITY_POS,
	.v_sync_pol = SPRDFB_POLARITY_POS,
	.de_pol = SPRDFB_POLARITY_POS,
	.te_pol = SPRDFB_POLARITY_POS,
	.color_mode_pol = SPRDFB_POLARITY_NEG,
	.shut_down_pol = SPRDFB_POLARITY_NEG,
	.timing = &lcd_jd9365_mipi_timing,
	.ops = NULL,
};

struct panel_spec lcd_jd9365_mipi_spec = {
	//.cap = PANEL_CAP_NOT_TEAR_SYNC,
	.width = 720,
	.height = 1280,
	.fps = 60,
	.type = LCD_MODE_DSI,
	.direction = LCD_DIRECT_NORMAL,
	.info = {
		.mipi = &lcd_jd9365_mipi_info
	},
	.ops = &lcd_jd9365_mipi_operations,
};


