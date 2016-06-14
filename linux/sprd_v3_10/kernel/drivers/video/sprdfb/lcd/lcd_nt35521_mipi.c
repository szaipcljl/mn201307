/* drivers/video/sc8825/lcd_nt35521_mipi.c
 *
 * Support for nt35521 mipi LCD device
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
#include <linux/delay.h>
#include "../sprdfb_panel.h"

//#define LCD_Delay(ms)  uDelay(ms*1000)

#define MAX_DATA   56

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

#if 1 //old
	//nt35521s-auo4.99-rixin-20160510
	{LCM_SEND(7),{5,0,0xFF,0xAA,0x55,0x25,0x01}},
	{LCM_SEND(2),{0x6F,0x21}},
	{LCM_SEND(2),{0xF7,0x01}},
	//{LCM_SEND(2),{REGFLAG_DELAY, 10, }},
	{LCM_SEND(2),{0x6F,0x21}},
	{LCM_SEND(2),{0xF7,0x00}},
	{LCM_SEND(7),{5,0,0xFF,0xAA,0x55,0x25,0x00}},

	{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x00}},
	{LCM_SEND(7),{5,0,0xFF,0xAA,0x55,0xA5,0x80}},
	{LCM_SEND(5),{3,0,0xB1,0x68,0x21}},
	{LCM_SEND(8),{6,0,0xBD,0x01,0xA3,0x10,0x10,0x01}},
	{LCM_SEND(2),{0x6F,0x02}},
	{LCM_SEND(2),{0xB8,0x08}},
	{LCM_SEND(5),{3,0,0xBB,0x11,0x11}},
	{LCM_SEND(5),{3,0,0xBC,0x05,0x05}},
	{LCM_SEND(2),{0xB6,0x01}},
	{LCM_SEND(2),{0xC8,0x83}},

	{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x01}},
	{LCM_SEND(5),{3,0,0xB0,0x0F,0x0F}},
	{LCM_SEND(5),{3,0,0xB1,0x0F,0x0F}},
	{LCM_SEND(2),{0xCE,0x66}},
	{LCM_SEND(2),{0xC0,0x0C}},
	{LCM_SEND(5),{3,0,0xB5,0x05,0x05}},
	{LCM_SEND(2),{0xBE,0x6a}},//3e
	{LCM_SEND(5),{3,0,0xB3,0x28,0x28}},
	{LCM_SEND(5),{3,0,0xB4,0x19,0x19}},

	{LCM_SEND(5),{3,0,0xB9,0x34,0x34}},//44
	{LCM_SEND(5),{3,0,0xBA,0x14,0x14}},
	{LCM_SEND(5),{3,0,0xBC,0x88,0x00}},//88
	{LCM_SEND(5),{3,0,0xBD,0x88,0x00}},//88
	{LCM_SEND(2),{0xCA,0x00}},

	{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x02}},
	{LCM_SEND(2),{0xEE,0x00}},
	//3Gamma 2.2
	//R(+) MCR cmd
	{LCM_SEND(19),{17,0,0xB0,0x00,0x79,0x00,0x8B,0x00,0xA5,0x00,0xB1,0x00,0xBC,0x00,0xD5,0x00,0xE9,0x01,0x0E}},
	{LCM_SEND(19),{17,0,0xB1,0x01,0x31,0x01,0x63,0x01,0x8F,0x01,0xD5,0x02,0x10,0x02,0x15,0x02,0x4E,0x02,0x8F}},
	{LCM_SEND(19),{17,0,0xB2,0x02,0xB6,0x02,0xEA,0x03,0x0D,0x03,0x3A,0x03,0x54,0x03,0x63,0x03,0x8F,0x03,0x94}},
	{LCM_SEND(7),{5,0,0xB3,0x03,0x96,0x03,0x97}},
	//G(+) MCR cmd
	{LCM_SEND(19),{17,0,0xB4,0x00,0x79,0x00,0x82,0x00,0x95,0x00,0xA5,0x00,0xB4,0x00,0xCE,0x00,0xE6,0x01,0x0D}},
	{LCM_SEND(19),{17,0,0xB5,0x01,0x2D,0x01,0x63,0x01,0x8F,0x01,0xD5,0x02,0x13,0x02,0x14,0x02,0x4F,0x02,0x8D}},
	{LCM_SEND(19),{17,0,0xB6,0x02,0xB5,0x02,0xEA,0x03,0x0D,0x03,0x39,0x03,0x55,0x03,0x68,0x03,0x93,0x03,0x9A}},
	{LCM_SEND(7),{5,0,0xB7,0x03,0xA4,0x03,0xA6}},
	//B(+) MCR cmd
	{LCM_SEND(19),{17,0,0xB8,0x00,0x60,0x00,0x6B,0x00,0x7E,0x00,0x8E,0x00,0x9D,0x00,0xB7,0x00,0xCE,0x00,0xF7}},
	{LCM_SEND(19),{17,0,0xB9,0x01,0x1A,0x01,0x53,0x01,0x82,0x01,0xCC,0x02,0x0C,0x02,0x0F,0x02,0x4C,0x02,0x8E}},
	{LCM_SEND(19),{17,0,0xBA,0x02,0xB7,0x02,0xF2,0x03,0x1B,0x03,0x5C,0x03,0x9C,0x03,0xFC,0x03,0xFC,0x03,0xFD}},
	{LCM_SEND(7),{5,0,0xBB,0x03,0xFE,0x03,0xFE}},
	//R(-) MCR cmd
	{LCM_SEND(19),{17,0,0xBC,0x00,0x79,0x00,0x8B,0x00,0xA5,0x00,0xB1,0x00,0xBC,0x00,0xD5,0x00,0xE9,0x01,0x0E}},
	{LCM_SEND(19),{17,0,0xBD,0x01,0x31,0x01,0x63,0x01,0x8F,0x01,0xD5,0x02,0x10,0x02,0x15,0x02,0x4E,0x02,0x8F}},
	{LCM_SEND(19),{17,0,0xBE,0x02,0xB6,0x02,0xEA,0x03,0x0D,0x03,0x3A,0x03,0x54,0x03,0x63,0x03,0x8F,0x03,0x94}},
	{LCM_SEND(7),{5,0,0xBF,0x03,0x96,0x03,0x97}},
	//G(-) MCR cmd
	{LCM_SEND(19),{17,0,0xC0,0x00,0x79,0x00,0x82,0x00,0x95,0x00,0xA5,0x00,0xB4,0x00,0xCE,0x00,0xE6,0x01,0x0D}},
	{LCM_SEND(19),{17,0,0xC1,0x01,0x2D,0x01,0x63,0x01,0x8F,0x01,0xD5,0x02,0x13,0x02,0x14,0x02,0x4F,0x02,0x8D}},
	{LCM_SEND(19),{17,0,0xC2,0x02,0xB5,0x02,0xEA,0x03,0x0D,0x03,0x39,0x03,0x55,0x03,0x68,0x03,0x93,0x03,0x9A}},
	{LCM_SEND(7),{5,0,0xC3,0x03,0xA4,0x03,0xA6}},
	//B(-) MCR cmd
	{LCM_SEND(19),{17,0,0xC4,0x00,0x60,0x00,0x6B,0x00,0x7E,0x00,0x8E,0x00,0x9D,0x00,0xB7,0x00,0xCE,0x00,0xF7}},
	{LCM_SEND(19),{17,0,0xC5,0x01,0x1A,0x01,0x53,0x01,0x82,0x01,0xCC,0x02,0x0C,0x02,0x0F,0x02,0x4C,0x02,0x8E}},
	{LCM_SEND(19),{17,0,0xC6,0x02,0xB7,0x02,0xF2,0x03,0x1B,0x03,0x5C,0x03,0x9C,0x03,0xFC,0x03,0xFC,0x03,0xFD}},
	{LCM_SEND(7),{5,0,0xC7,0x03,0xFE,0x03,0xFE}},


	{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x03}},
	{LCM_SEND(5),{3,0,0xB0,0x20,0x00}},
	{LCM_SEND(5),{3,0,0xB1,0x20,0x00}},
	{LCM_SEND(8),{6,0,0xB2,0x04,0x00,0x52,0x01,0x51}},
	{LCM_SEND(8),{6,0,0xB3,0x04,0x00,0x52,0x01,0x51}},
	{LCM_SEND(8),{6,0,0xB6,0x04,0x00,0x52,0x01,0x51}},
	{LCM_SEND(8),{6,0,0xB7,0x04,0x00,0x52,0x01,0x51}},
	{LCM_SEND(8),{6,0,0xBA,0x44,0x00,0x60,0x01,0x72}},
	{LCM_SEND(8),{6,0,0xBB,0x44,0x00,0x60,0x01,0x72}},
	{LCM_SEND(8),{6,0,0xBC,0x53,0x00,0x03,0x00,0x48}},
	{LCM_SEND(8),{6,0,0xBD,0x53,0x00,0x03,0x00,0x48}},
	{LCM_SEND(2),{0xC4,0x40}},
	{LCM_SEND(2),{0xC5,0x40}},

	{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x05}},
	{LCM_SEND(5),{3,0,0xB0,0x17,0x06}},
	{LCM_SEND(2),{0xB8,0x00}},
	{LCM_SEND(8),{6,0,0xBD,0x0F,0x03,0x03,0x00,0x03}},
	{LCM_SEND(5),{3,0,0xB1,0x17,0x06}},
	{LCM_SEND(2),{0xB9,0x00}},
	{LCM_SEND(5),{3,0,0xB2,0x17,0x06}},
	{LCM_SEND(2),{0xBA,0x00}},
	{LCM_SEND(5),{3,0,0xB3,0x17,0x06}},
	{LCM_SEND(2),{0xBB,0x00}},
	{LCM_SEND(5),{3,0,0xB4,0x17,0x06}},
	{LCM_SEND(5),{3,0,0xB5,0x17,0x06}},
	{LCM_SEND(5),{3,0,0xB6,0x17,0x06}},
	{LCM_SEND(5),{3,0,0xB7,0x17,0x06}},
	{LCM_SEND(2),{0xBC,0x00}},
	{LCM_SEND(2),{0xE5,0x06}},
	{LCM_SEND(2),{0xE6,0x06}},
	{LCM_SEND(2),{0xE7,0x06}},
	{LCM_SEND(2),{0xE8,0x06}},
	{LCM_SEND(2),{0xE9,0x0A}},
	{LCM_SEND(2),{0xEA,0x06}},
	{LCM_SEND(2),{0xEB,0x06}},
	{LCM_SEND(2),{0xEC,0x06}},
	{LCM_SEND(2),{0xED,0x30}},
	{LCM_SEND(2),{0xC0,0x07}},
	{LCM_SEND(2),{0xC1,0x05}},
	{LCM_SEND(2),{0xC4,0x82}},
	{LCM_SEND(2),{0xC5,0x80}},
	{LCM_SEND(5),{3,0,0xC8,0x03,0x20}},
	{LCM_SEND(5),{3,0,0xC9,0x01,0x21}},
	{LCM_SEND(5),{3,0,0xCA,0x03,0x20}},
	{LCM_SEND(5),{3,0,0xCB,0x07,0x20}},
	{LCM_SEND(14),{12,0,0xD1,0x03,0x05,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	{LCM_SEND(14),{12,0,0xD2,0x03,0x05,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	{LCM_SEND(14),{12,0,0xD3,0x00,0x05,0x04,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	{LCM_SEND(14),{12,0,0xD4,0x00,0x05,0x04,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x06}},
	{LCM_SEND(5),{3,0,0xB0,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xB1,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xB2,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xB3,0x2E,0x09}},
	{LCM_SEND(5),{3,0,0xB4,0x0B,0x23}},
	{LCM_SEND(5),{3,0,0xB5,0x1d,0x1f}},
	{LCM_SEND(5),{3,0,0xB6,0x11,0x17}},
	{LCM_SEND(5),{3,0,0xB7,0x13,0x19}},
	{LCM_SEND(5),{3,0,0xB8,0x01,0x03}},
	{LCM_SEND(5),{3,0,0xB9,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xBA,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xBB,0x02,0x00}},
	{LCM_SEND(5),{3,0,0xBC,0x18,0x12}},
	{LCM_SEND(5),{3,0,0xBD,0x16,0x10}},
	{LCM_SEND(5),{3,0,0xBE,0x1e,0x1c}},
	{LCM_SEND(5),{3,0,0xBF,0x22,0x0A}},
	{LCM_SEND(5),{3,0,0xC0,0x08,0x2E}},
	{LCM_SEND(5),{3,0,0xC1,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xC2,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xC3,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xE5,0x25,0x24}},
	{LCM_SEND(5),{3,0,0xC4,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xC5,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xC6,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xC7,0x2E,0x02}},
	{LCM_SEND(5),{3,0,0xC8,0x00,0x24}},
	{LCM_SEND(5),{3,0,0xC9,0x1E,0x1C}},
	{LCM_SEND(5),{3,0,0xCA,0x18,0x12}},
	{LCM_SEND(5),{3,0,0xCB,0x16,0x10}},
	{LCM_SEND(5),{3,0,0xCC,0x0A,0x08}},
	{LCM_SEND(5),{3,0,0xCD,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xCE,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xCF,0x09,0x0B}},
	{LCM_SEND(5),{3,0,0xD0,0x11,0x17}},
	{LCM_SEND(5),{3,0,0xD1,0x13,0x19}},
	{LCM_SEND(5),{3,0,0xD2,0x1D,0x1F}},
	{LCM_SEND(5),{3,0,0xD3,0x25,0x01}},
	{LCM_SEND(5),{3,0,0xD4,0x03,0x2E}},
	{LCM_SEND(5),{3,0,0xD5,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xD6,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xD7,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xE6,0x22,0x23}},
	{LCM_SEND(8),{6,0,0xD8,0x00,0x00,0x00,0x00,0x00}},
	{LCM_SEND(8),{6,0,0xD9,0x00,0x00,0x00,0x00,0x00}},
	{LCM_SEND(2),{0xE7,0x00}},
	//BIST MODE
	//{0xFF,4,{0xAA,0x55,0x25,0x01}},//NT35521S 3LINE  
	//{0x6F,1,{0x16}},
	//{0xF7,1,{0x10}},

	{LCM_SEND(2),{0x11,0x00}},
	{LCM_SLEEP(120)},

	{LCM_SEND(2),{0x29,0x00}},
	{LCM_SLEEP(50)},
#else
	//****NT35521S-AUO4.95-MIPI-RIXIN-20151112******
	{LCM_SEND(7),{5,0,0xFF,0xAA,0x55,0x25,0x01}},
	{LCM_SEND(2),{0x6F,0x21}},
	{LCM_SEND(2),{0xF7,0x01}},
	//{LCM_SEND(2),{REGFLAG_DELAY, 10, }},
	{LCM_SEND(2),{0x6F,0x21}},
	{LCM_SEND(2),{0xF7,0x00}},
	{LCM_SEND(7),{5,0,0xFF,0xAA,0x55,0x25,0x00}},

	{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x00}},
	{LCM_SEND(5),{3,0,0xB1,0x68,0x21}},
	{LCM_SEND(2),{0xB6,0x01}},
	{LCM_SEND(7),{5,0,0xB8,0x01,0x02,0x08,0x02}},
	{LCM_SEND(5),{3,0,0xBB,0x11,0x11}},
	{LCM_SEND(5),{3,0,0xBC,0x00,0x00}},
	{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x01}},
	{LCM_SEND(5),{3,0,0xB0,0x0F,0x0F}},//0F
	{LCM_SEND(5),{3,0,0xB1,0x0F,0x0F}},//0F
	{LCM_SEND(5),{3,0,0xB3,0x28,0x28}},
	{LCM_SEND(5),{3,0,0xB4,0x19,0x19}},
	{LCM_SEND(5),{3,0,0xB5,0x05,0x05}},
	{LCM_SEND(5),{3,0,0xB9,0x24,0x24}},//44
	{LCM_SEND(5),{3,0,0xBA,0x14,0x14}},//24
	{LCM_SEND(5),{3,0,0xBC,0x70,0x00}},//88
	{LCM_SEND(5),{3,0,0xBD,0x70,0x00}},//88
	{LCM_SEND(2),{0xBE,0x44}},
	{LCM_SEND(2),{0xC0,0x0C}},
	{LCM_SEND(2),{0xCA,0x00}},
	{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x02}},
	{LCM_SEND(2),{0xEE,0x00}},
	{LCM_SEND(19),{17,0,0xB0,0x00,0x91,0x00,0xA0,0x00,0xBC,0x00,0xD3,0x00,0xE6,0x01,0x07,0x01
					  ,0x1F,0x01,0x4E}},

	{LCM_SEND(19),{17,0,0xB1,0x01,0x72,0x01,0xAB,0x01,0xD6,0x02,0x1D,0x02,0x51,0x02,0x53,0x02
					  ,0x85,0x02,0xBE}},

	{LCM_SEND(19),{17,0,0xB2,0x02,0xDE,0x03,0x0E,0x03,0x2E,0x03,0x52,0x03,0x70,0x03,0x91,0x03
					  ,0xA1,0x03,0xB3}},

	{LCM_SEND(7),{5,0,0xB3,0x03,0xC5,0x03,0xDF}},
	{LCM_SEND(19),{17,0,0xB4,0x00,0x91,0x00,0xA0,0x00,0xBC,0x00,0xD3,0x00,0xE6,0x01,0x07,0x01
					  ,0x1F,0x01,0x4E}},

	{LCM_SEND(19),{17,0,0xB5,0x01,0x72,0x01,0xAB,0x01,0xD6,0x02,0x1D,0x02,0x51,0x02,0x53,0x02
					  ,0x85,0x02,0xBE}},

	{LCM_SEND(19),{17,0,0xB6,0x02,0xDE,0x03,0x0E,0x03,0x2E,0x03,0x52,0x03,0x70,0x03,0x91,0x03
					  ,0xA1,0x03,0xB3}},

	{LCM_SEND(7),{5,0,0xB7,0x03,0xC5,0x03,0xDF}},
	{LCM_SEND(19),{17,0,0xB8,0x00,0x91,0x00,0xA0,0x00,0xBC,0x00,0xD3,0x00,0xE6,0x01,0x07,0x01
					  ,0x1F,0x01,0x4E}},

	{LCM_SEND(19),{17,0,0xB9,0x01,0x72,0x01,0xAB,0x01,0xD6,0x02,0x1D,0x02,0x51,0x02,0x53,0x02
					  ,0x85,0x02,0xBE}},

	{LCM_SEND(19),{17,0,0xBA,0x02,0xDE,0x03,0x0E,0x03,0x2E,0x03,0x52,0x03,0x70,0x03,0x91,0x03
					  ,0xA1,0x03,0xB3}},

	{LCM_SEND(7),{5,0,0xBB,0x03,0xC5,0x03,0xDF}},
	{LCM_SEND(19),{17,0,0xBC,0x00,0x49,0x00,0x63,0x00,0x86,0x00,0xA1,0x00,0xB8,0x00,0xDF,0x00
					  ,0xFD,0x01,0x30}},

	{LCM_SEND(19),{17,0,0xBD,0x01,0x5A,0x01,0x97,0x01,0xC9,0x02,0x15,0x02,0x4F,0x02,0x53,0x02
					  ,0x85,0x02,0xBE}},

	{LCM_SEND(19),{17,0,0xBE,0x02,0xDE,0x03,0x0E,0x03,0x2E,0x03,0x52,0x03,0x70,0x03,0x91,0x03
					  ,0xA1,0x03,0xB3}},

	{LCM_SEND(7),{5,0,0xBF,0x03,0xC5,0x03,0xF5}},
	{LCM_SEND(19),{17,0,0xC0,0x00,0x49,0x00,0x63,0x00,0x86,0x00,0xA1,0x00,0xB8,0x00,0xDF,0x00
					  ,0xFD,0x01,0x30}},

	{LCM_SEND(19),{17,0,0xC1,0x01,0x5A,0x01,0x97,0x01,0xC9,0x02,0x15,0x02,0x4F,0x02,0x53,0x02
					  ,0x85,0x02,0xBE}},

	{LCM_SEND(19),{17,0,0xC2,0x02,0xDE,0x03,0x0E,0x03,0x2E,0x03,0x52,0x03,0x70,0x03,0x91,0x03
					  ,0xA1,0x03,0xB3}},

	{LCM_SEND(7),{5,0,0xC3,0x03,0xC5,0x03,0xF5}},
	{LCM_SEND(19),{17,0,0xC4,0x00,0x49,0x00,0x63,0x00,0x86,0x00,0xA1,0x00,0xB8,0x00,0xDF,0x00
					  ,0xFD,0x01,0x30}},

	{LCM_SEND(19),{17,0,0xC5,0x01,0x5A,0x01,0x97,0x01,0xC9,0x02,0x15,0x02,0x4F,0x02,0x53,0x02
					  ,0x85,0x02,0xBE}},

	{LCM_SEND(19),{17,0,0xC6,0x02,0xDE,0x03,0x0E,0x03,0x2E,0x03,0x52,0x03,0x70,0x03,0x91,0x03
					  ,0xA1,0x03,0xB3}},

	{LCM_SEND(7),{5,0,0xC7,0x03,0xC5,0x03,0xF5}},
	{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x03}},
	{LCM_SEND(5),{3,0,0xB0,0x20,0x00}},
	{LCM_SEND(5),{3,0,0xB1,0x20,0x00}},
	{LCM_SEND(8),{6,0,0xB2,0x04,0x00,0x52,0x01,0x51}},
	{LCM_SEND(8),{6,0,0xB3,0x04,0x00,0x52,0x01,0x51}},
	{LCM_SEND(8),{6,0,0xB6,0x04,0x00,0x52,0x01,0x51}},
	{LCM_SEND(8),{6,0,0xB7,0x04,0x00,0x52,0x01,0x51}},
	{LCM_SEND(8),{6,0,0xBA,0x44,0x00,0x60,0x01,0x72}},
	{LCM_SEND(8),{6,0,0xBB,0x44,0x00,0x60,0x01,0x72}},
	{LCM_SEND(8),{6,0,0xBC,0x53,0x00,0x03,0x00,0x48}},
	{LCM_SEND(8),{6,0,0xBD,0x53,0x00,0x03,0x00,0x48}},
	{LCM_SEND(2),{0xC4,0x40}},
	{LCM_SEND(2),{0xC5,0x40}},
	{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x05}},
	{LCM_SEND(5),{3,0,0xB0,0x17,0x06}},
	{LCM_SEND(2),{0xB8,0x00}},
	{LCM_SEND(8),{6,0,0xBD,0x0F,0x03,0x03,0x00,0x03}},
	{LCM_SEND(5),{3,0,0xB1,0x17,0x06}},
	{LCM_SEND(2),{0xB9,0x00}},
	{LCM_SEND(5),{3,0,0xB2,0x17,0x06}},
	{LCM_SEND(2),{0xBA,0x00}},
	{LCM_SEND(5),{3,0,0xB3,0x17,0x06}},
	{LCM_SEND(2),{0xBB,0x00}},
	{LCM_SEND(5),{3,0,0xB4,0x17,0x06}},
	{LCM_SEND(5),{3,0,0xB5,0x17,0x06}},
	{LCM_SEND(5),{3,0,0xB6,0x17,0x06}},
	{LCM_SEND(5),{3,0,0xB7,0x17,0x06}},
	{LCM_SEND(2),{0xBC,0x00}},
	{LCM_SEND(2),{0xE5,0x06}},
	{LCM_SEND(2),{0xE6,0x06}},
	{LCM_SEND(2),{0xE7,0x06}},
	{LCM_SEND(2),{0xE8,0x06}},
	{LCM_SEND(2),{0xE9,0x0A}},
	{LCM_SEND(2),{0xEA,0x06}},
	{LCM_SEND(2),{0xEB,0x06}},
	{LCM_SEND(2),{0xEC,0x06}},
	{LCM_SEND(2),{0xED,0x30}},
	{LCM_SEND(2),{0xC0,0x07}},
	{LCM_SEND(2),{0xC1,0x05}},
	{LCM_SEND(2),{0xC4,0x82}},
	{LCM_SEND(2),{0xC5,0x80}},
	{LCM_SEND(5),{3,0,0xC8,0x03,0x20}},
	{LCM_SEND(5),{3,0,0xC9,0x01,0x21}},
	{LCM_SEND(5),{3,0,0xCA,0x03,0x20}},
	{LCM_SEND(5),{3,0,0xCB,0x07,0x20}},
	{LCM_SEND(14),{12,0,0xD1,0x03,0x05,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	{LCM_SEND(14),{12,0,0xD2,0x03,0x05,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	{LCM_SEND(14),{12,0,0xD3,0x00,0x05,0x04,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	{LCM_SEND(14),{12,0,0xD4,0x00,0x05,0x04,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x06}},
	{LCM_SEND(5),{3,0,0xB0,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xB1,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xB2,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xB3,0x2E,0x09}},
	{LCM_SEND(5),{3,0,0xB4,0x0B,0x23}},
	{LCM_SEND(5),{3,0,0xB5,0x1d,0x1f}},
	{LCM_SEND(5),{3,0,0xB6,0x11,0x17}},
	{LCM_SEND(5),{3,0,0xB7,0x13,0x19}},
	{LCM_SEND(5),{3,0,0xB8,0x01,0x03}},
	{LCM_SEND(5),{3,0,0xB9,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xBA,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xBB,0x02,0x00}},
	{LCM_SEND(5),{3,0,0xBC,0x18,0x12}},
	{LCM_SEND(5),{3,0,0xBD,0x16,0x10}},
	{LCM_SEND(5),{3,0,0xBE,0x1e,0x1c}},
	{LCM_SEND(5),{3,0,0xBF,0x22,0x0A}},
	{LCM_SEND(5),{3,0,0xC0,0x08,0x2E}},
	{LCM_SEND(5),{3,0,0xC1,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xC2,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xC3,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xE5,0x25,0x24}},
	{LCM_SEND(5),{3,0,0xC4,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xC5,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xC6,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xC7,0x2E,0x02}},
	{LCM_SEND(5),{3,0,0xC8,0x00,0x24}},
	{LCM_SEND(5),{3,0,0xC9,0x1E,0x1C}},
	{LCM_SEND(5),{3,0,0xCA,0x18,0x12}},
	{LCM_SEND(5),{3,0,0xCB,0x16,0x10}},
	{LCM_SEND(5),{3,0,0xCC,0x0A,0x08}},
	{LCM_SEND(5),{3,0,0xCD,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xCE,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xCF,0x09,0x0B}},
	{LCM_SEND(5),{3,0,0xD0,0x11,0x17}},
	{LCM_SEND(5),{3,0,0xD1,0x13,0x19}},
	{LCM_SEND(5),{3,0,0xD2,0x1D,0x1F}},
	{LCM_SEND(5),{3,0,0xD3,0x25,0x01}},
	{LCM_SEND(5),{3,0,0xD4,0x03,0x2E}},
	{LCM_SEND(5),{3,0,0xD5,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xD6,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xD7,0x2E,0x2E}},
	{LCM_SEND(5),{3,0,0xE6,0x22,0x23}},
	{LCM_SEND(8),{6,0,0xD8,0x00,0x00,0x00,0x00,0x00}},
	{LCM_SEND(8),{6,0,0xD9,0x00,0x00,0x00,0x00,0x00}},
	{LCM_SEND(2),{0xE7,0x00}},

	//{0xFF,4,{0xAA,0x55,0x25,0x01}},//NT35521S 3LINE  
	//{0x6F,1,{0x16}},
	//{0xF7,1,{0x10}},

	{LCM_SEND(2),{0x11,0x00}},
	{LCM_SLEEP(120)},

	{LCM_SEND(2),{0x29,0x00}},
	{LCM_SLEEP(50)},

#endif
};

//static LCM_Init_Code disp_on =  {LCM_SEND(1), {0x29}};

static LCM_Init_Code sleep_in[] =  {
	{LCM_SEND(1), {0x28}},
	{LCM_SLEEP(50)},
	{LCM_SEND(1), {0x10}},
	{LCM_SLEEP(200)},
	//{LCM_SEND(2), {0x4f, 0x01}},
};

static LCM_Init_Code sleep_out[] =  {
	{LCM_SEND(1), {0x11}},
	{LCM_SLEEP(150)},
	{LCM_SEND(1), {0x29}},
	{LCM_SLEEP(10)},
};

static int32_t nt35521_mipi_init(struct panel_spec *self)
{
	int32_t i = 0;
	LCM_Init_Code *init = init_data;
	unsigned int tag;

	mipi_set_cmd_mode_t mipi_set_cmd_mode = self->info.mipi->ops->mipi_set_cmd_mode;
	mipi_gen_write_t mipi_gen_write = self->info.mipi->ops->mipi_gen_write;
	mipi_dcs_write_t mipi_dcs_write = self->info.mipi->ops->mipi_dcs_write;

	pr_debug(KERN_DEBUG "nt35521_mipi_init\n");

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

static LCM_Force_Cmd_Code rd_prep_code[]={
	{0x39, {LCM_SEND(8), {0x6, 0, 0xF0, 0x55, 0xAA, 0x52, 0x08, 0x01}}},
	{0x37, {LCM_SEND(2), {0x3, 0}}},
};

static uint32_t nt35521_readid(struct panel_spec *self)
{
	//	return 0x5521;
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

	printk("lcd_nt35521_mipi read id!\n");
	//return 0x5521;	//fix lcd id read problem

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

		read_rtn = mipi_force_read(0xc5, 3,(uint8_t *)read_data);
		printk("lcd_nt35521_mipi read id 0xc5 value is 0x%x, 0x%x, 0x%x!\n", read_data[0], read_data[1], read_data[2]);
		if((0x55 == read_data[0])&&(0x21 == read_data[1])){
			printk("lcd_nt35521_mipi read id success!\n");
			mipi_eotp_set(1,1);
			return 0x5521;
		}
	}
	mipi_eotp_set(1,1);

	return 0;





}

static int32_t nt35521_enter_sleep(struct panel_spec *self, uint8_t is_sleep)
{
	int32_t i = 0;
	LCM_Init_Code *sleep_in_out = NULL;
	unsigned int tag;
	int32_t size = 0;

	mipi_dcs_write_t mipi_dcs_write = self->info.mipi->ops->mipi_dcs_write;
	mipi_gen_write_t mipi_gen_write = self->info.mipi->ops->mipi_gen_write;

	printk(KERN_DEBUG "nt35521_enter_sleep, is_sleep = %d\n", is_sleep);

	if(is_sleep){
		sleep_in_out = sleep_in;
		size = ARRAY_SIZE(sleep_in);
	}else{
		sleep_in_out = sleep_out;
		size = ARRAY_SIZE(sleep_out);
	}

	for(i = 0; i <size ; i++){
		tag = (sleep_in_out->tag >>24);
		if(tag & LCM_TAG_SEND){
			mipi_gen_write(sleep_in_out->data, (sleep_in_out->tag & LCM_TAG_MASK));
		}else if(tag & LCM_TAG_SLEEP){
			msleep((sleep_in_out->tag & LCM_TAG_MASK));
		}
		sleep_in_out++;
	}
	return 0;
}


static LCM_Force_Cmd_Code rd_prep_code_1[]={
	{0x37, {LCM_SEND(2), {0x1, 0}}},
};

static uint32_t nt35521_readpowermode(struct panel_spec *self)
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

	pr_debug("lcd_nt35521_mipi read power mode!\n");
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
		//printk("lcd_nt35521 mipi read power mode 0x0A value is 0x%x! , read result(%d)\n", read_data[0], read_rtn);
		if((0x9c == read_data[0])  && (0 == read_rtn)){
			pr_debug("lcd_nt35521_mipi read power mode success!\n");
			mipi_eotp_set(1,1);
			return 0x9c;
		}
	}

	printk("lcd_nt35521 mipi read power mode fail!0x0A value is 0x%x! , read result(%d)\n", read_data[0], read_rtn);
	mipi_eotp_set(1,1);
	return 0x0;
}
static int32_t nt35521_check_esd(struct panel_spec *self)
{
	uint32_t power_mode;

	mipi_set_lp_mode_t mipi_set_data_lp_mode = self->info.mipi->ops->mipi_set_data_lp_mode;
	mipi_set_hs_mode_t mipi_set_data_hs_mode = self->info.mipi->ops->mipi_set_data_hs_mode;
	mipi_set_lp_mode_t mipi_set_lp_mode = self->info.mipi->ops->mipi_set_lp_mode;
	mipi_set_hs_mode_t mipi_set_hs_mode = self->info.mipi->ops->mipi_set_hs_mode;
	uint16_t work_mode = self->info.mipi->work_mode;

	printk("nt35521_check_esd!\n");
#ifndef FB_CHECK_ESD_IN_VFP
	if(SPRDFB_MIPI_MODE_CMD==work_mode){
		mipi_set_lp_mode();
	}else{
		mipi_set_data_lp_mode();
	}
#endif
	power_mode = nt35521_readpowermode(self);
	//power_mode = 0x0;
#ifndef FB_CHECK_ESD_IN_VFP
	if(SPRDFB_MIPI_MODE_CMD==work_mode){
		mipi_set_hs_mode();
	}else{
		mipi_set_data_hs_mode();
	}
#endif
	if(power_mode == 0x9c){
		printk("nt35521_check_esd OK!\n");
		return 1;
	}else{
		printk("nt35521_check_esd fail!(0x%x)\n", power_mode);
		return 0;
	}
}


static int32_t nt35521_after_suspend(struct panel_spec *self)
{
    // Do nothing;

    return 0;

}

static struct panel_operations lcd_nt35521_mipi_operations = {
	.panel_init = nt35521_mipi_init,
	.panel_readid = nt35521_readid,
	.panel_enter_sleep = nt35521_enter_sleep,
	.panel_esd_check = nt35521_check_esd,	
	.panel_after_suspend = nt35521_after_suspend,
};

static struct timing_rgb lcd_nt35521_mipi_timing = {
	.hfp = 72,  /* unit: pixel */// 100
	.hbp = 72,//80
	.hsync = 10,//6
	.vfp = 40, /*unit: line*/
	.vbp = 38,
	.vsync = 4, //6,
};

static struct info_mipi lcd_nt35521_mipi_info = {
	.work_mode  = SPRDFB_MIPI_MODE_VIDEO, //SPRDFB_MIPI_MODE_VIDEO  SPRDFB_MIPI_MODE_CMD
	.video_bus_width = 24, /*18,16*/
	.lan_number = 4,
	.phy_feq = 500*1000,
	.h_sync_pol = SPRDFB_POLARITY_POS,
	.v_sync_pol = SPRDFB_POLARITY_POS,
	.de_pol = SPRDFB_POLARITY_POS,
	.te_pol = SPRDFB_POLARITY_POS,
	.color_mode_pol = SPRDFB_POLARITY_NEG,
	.shut_down_pol = SPRDFB_POLARITY_NEG,
	.timing = &lcd_nt35521_mipi_timing,
	.ops = NULL,
};

struct panel_spec lcd_nt35521_mipi_spec = {
	//.cap = PANEL_CAP_NOT_TEAR_SYNC,
	.width = 720,
	.height = 1280,
	.fps = 60,	
	.type = LCD_MODE_DSI,
	.direction = LCD_DIRECT_NORMAL,
	.suspend_mode = SEND_SLEEP_CMD,
	.is_clean_lcd = true,
	.info = {
		.mipi = &lcd_nt35521_mipi_info
	},
	.ops = &lcd_nt35521_mipi_operations,
};


struct panel_cfg lcd_nt35521_mipi = {
	/* this panel can only be main lcd */
	.dev_id = SPRDFB_MAINLCD_ID,
	.lcd_id = 0x5521,
	.lcd_name = "lcd_nt35521_mipi",
	.panel = &lcd_nt35521_mipi_spec,
};

static int __init lcd_nt35521_mipi_init(void)
{
	return sprdfb_panel_register(&lcd_nt35521_mipi);
}

subsys_initcall(lcd_nt35521_mipi_init);
