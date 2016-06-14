/* drivers/video/sc8825/lcd_rm68191_mipi.c
 *
 * Support for rm68191 mipi LCD device
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
#if 1 //for haifei, add 150917

	{LCM_SEND(8),{6,0,0xF0,0x55, 0xAA, 0x52, 0x08, 0x03}},

	{LCM_SEND(12),{10,0,0x90,0x03, 0x14, 0x09, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00}},

	{LCM_SEND(12),{10,0,0x91,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},

	{LCM_SEND(14),{12,0,0x92,0x40, 0x0B, 0x0C, 0x0D, 0x0E, 0x00, 0x38, 0x00, 0x10, 0x03, 0x04}},

	{LCM_SEND(11),{9,0,0x94,0x00, 0x08, 0x0B, 0x03, 0xD2, 0x03, 0xD3, 0x0C}},

	{LCM_SEND(19),{17,0,0x95,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},

	{LCM_SEND(5),{3,0,0x99,0x00, 0x00}},

	{LCM_SEND(14),{12,0,0x9A,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},

	{LCM_SEND(9),{7,0,0x9B,0x01, 0x38, 0x00, 0x00, 0x00, 0x00}},

	{LCM_SEND(5),{3,0,0x9C,0x00, 0x00}},

	{LCM_SEND(11),{9,0,0x9D,0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00}},

	{LCM_SEND(5),{3,0,0x9E,0x00, 0x00}},

	{LCM_SEND(13),{11,0,0xA0,0x9F, 0x1F, 0x08, 0x1F, 0x0A, 0x1F, 0x00, 0x1F, 0x14, 0x1F}},

	{LCM_SEND(13),{11,0,0xA1,0x15, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F}},

	{LCM_SEND(13),{11,0,0xA2,0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F}},

	{LCM_SEND(13),{11,0,0xA4,0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F}},

	{LCM_SEND(13),{11,0,0xA5,0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x15}},

	{LCM_SEND(13),{11,0,0xA6,0x1F, 0x14, 0x1F, 0x01, 0x1F, 0x0B, 0x1F, 0x09, 0x1F, 0x1F}},

	{LCM_SEND(13),{11,0,0xA7,0x1F, 0x1F, 0x0B, 0x1F, 0x09, 0x1F, 0x01, 0x1F, 0x15, 0x1F}},

	{LCM_SEND(13),{11,0,0xA8,0x14, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F}},

	{LCM_SEND(13),{11,0,0xA9,0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F}},

	{LCM_SEND(13),{11,0,0xAB,0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F}},

	{LCM_SEND(13),{11,0,0xAC,0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x14}},

	{LCM_SEND(13),{11,0,0xAD,0x1F, 0x15, 0x1F, 0x00, 0x1F, 0x08, 0x1F, 0x0A, 0x1F, 0x1F}},

	{LCM_SEND(8),{6,0,0xF0,0x55, 0xAA, 0x52, 0x08, 0x00}},

	{LCM_SEND(6),{4,0,0xBC,0x00, 0x00, 0x00}},

	{LCM_SEND(7),{5,0,0xB8,0x01, 0xAF, 0x8F, 0x8F}},

	{LCM_SEND(8),{6,0,0xF0,0x55, 0xAA, 0x52, 0x08, 0x01}},

	{LCM_SEND(19),{17,0,0xD1,0x00, 0x00, 0x00, 0x0F, 0x00, 0x2A, 0x00, 0x41, 0x00, 0x54, 0x00, 0x75, 0x00, 0x92, 0x00, 0xC0}},

	{LCM_SEND(19),{17,0,0xD2,0x00, 0xE7, 0x01, 0x25, 0x01, 0x56, 0x01, 0xA6, 0x01, 0xE6, 0x01, 0xE8, 0x02, 0x24, 0x02, 0x63}},

	{LCM_SEND(19),{17,0,0xD3,0x02, 0x8D, 0x02, 0xC4, 0x02, 0xEC, 0x03, 0x23, 0x03, 0x48, 0x03, 0x78, 0x03, 0x97, 0x03, 0xBD}},

	{LCM_SEND(7),{5,0,0xD4,0x03, 0xE5, 0x03, 0xFF}},

	{LCM_SEND(19),{17,0,0xD5,0x00, 0x00, 0x00, 0x0F, 0x00, 0x2A, 0x00, 0x41, 0x00, 0x54, 0x00, 0x75, 0x00, 0x92, 0x00, 0xC0}},

	{LCM_SEND(19),{17,0,0xD6,0x00, 0xE7, 0x01, 0x25, 0x01, 0x56, 0x01, 0xA6, 0x01, 0xE6, 0x01, 0xE8, 0x02, 0x24, 0x02, 0x63}},

	{LCM_SEND(19),{17,0,0xD7,0x02, 0x8D, 0x02, 0xC4, 0x02, 0xEC, 0x03, 0x23, 0x03, 0x48, 0x03, 0x78, 0x03, 0x97, 0x03, 0xBD}},

	{LCM_SEND(7),{5,0,0xD8,0x03, 0xE5, 0x03, 0xFF}},

	{LCM_SEND(19),{17,0,0xD9,0x00, 0x00, 0x00, 0x0F, 0x00, 0x2A, 0x00, 0x41, 0x00, 0x54, 0x00, 0x75, 0x00, 0x92, 0x00, 0xC0}},

	{LCM_SEND(19),{17,0,0xDD,0x00, 0xE7, 0x01, 0x25, 0x01, 0x56, 0x01, 0xA6, 0x01, 0xE6, 0x01, 0xE8, 0x02, 0x24, 0x02, 0x63}},

	{LCM_SEND(19),{17,0,0xDE,0x02, 0x8D, 0x02, 0xC4, 0x02, 0xEC, 0x03, 0x23, 0x03, 0x48, 0x03, 0x78, 0x03, 0x97, 0x03, 0xBD}},

	{LCM_SEND(7),{5,0,0xDF,0x03, 0xE5, 0x03, 0xFF}},

	{LCM_SEND(19),{17,0,0xE0,0x00, 0x00, 0x00, 0x0F, 0x00, 0x2A, 0x00, 0x41, 0x00, 0x54, 0x00, 0x75, 0x00, 0x92, 0x00, 0xC0}},

	{LCM_SEND(19),{17,0,0xE1,0x00, 0xE7, 0x01, 0x25, 0x01, 0x56, 0x01, 0xA6, 0x01, 0xE6, 0x01, 0xE8, 0x02, 0x24, 0x02, 0x63}},

	{LCM_SEND(19),{17,0,0xE2,0x02, 0x8D, 0x02, 0xC4, 0x02, 0xEC, 0x03, 0x23, 0x03, 0x48, 0x03, 0x78, 0x03, 0x97, 0x03, 0xBD}},

	{LCM_SEND(7),{5,0,0xE3,0x03, 0xE5, 0x03, 0xFF}},

	{LCM_SEND(19),{17,0,0xE4,0x00, 0x00, 0x00, 0x0F, 0x00, 0x2A, 0x00, 0x41, 0x00, 0x54, 0x00, 0x75, 0x00, 0x92, 0x00, 0xC0}},

	{LCM_SEND(19),{17,0,0xE5,0x00, 0xE7, 0x01, 0x25, 0x01, 0x56, 0x01, 0xA6, 0x01, 0xE6, 0x01, 0xE8, 0x02, 0x24, 0x02, 0x63}},

	{LCM_SEND(19),{17,0,0xE6,0x02, 0x8D, 0x02, 0xC4, 0x02, 0xEC, 0x03, 0x23, 0x03, 0x48, 0x03, 0x78, 0x03, 0x97, 0x03, 0xBD}},

	{LCM_SEND(7),{5,0,0xE7,0x03, 0xE5, 0x03, 0xFF}},

	{LCM_SEND(19),{17,0,0xE8,0x00, 0x00, 0x00, 0x0F, 0x00, 0x2A, 0x00, 0x41, 0x00, 0x54, 0x00, 0x75, 0x00, 0x92, 0x00, 0xC0}},

	{LCM_SEND(19),{17,0,0xE9,0x00, 0xE7, 0x01, 0x25, 0x01, 0x56, 0x01, 0xA6, 0x01, 0xE6, 0x01, 0xE8, 0x02, 0x24, 0x02, 0x63}},

	{LCM_SEND(19),{17,0,0xEA,0x02, 0x8D, 0x02, 0xC4, 0x02, 0xEC, 0x03, 0x23, 0x03, 0x48, 0x03, 0x78, 0x03, 0x97, 0x03, 0xBD}},

	{LCM_SEND(7),{5,0,0xEB,0x03, 0xE5, 0x03, 0xFF}},

	{LCM_SEND(6),{4,0,0xB0,0x05, 0x05, 0x05}},

	{LCM_SEND(6),{4,0,0xB1,0x05, 0x05, 0x05}},

	{LCM_SEND(6),{4,0,0xB3,0x10, 0x10, 0x10}},

	{LCM_SEND(6),{4,0,0xB4,0x06, 0x06, 0x06}},

	{LCM_SEND(6),{4,0,0xB6,0x44, 0x44, 0x44}},

	{LCM_SEND(6),{4,0,0xB7,0x34, 0x34, 0x34}},

	{LCM_SEND(6),{4,0,0xB8,0x34, 0x34, 0x34}},

	{LCM_SEND(6),{4,0,0xB9,0x24, 0x24, 0x24}},

	{LCM_SEND(6),{4,0,0xBA,0x24, 0x24, 0x24}},

	{LCM_SEND(6),{4,0,0xBC,0x00, 0x6c, 0x00}},//70

	{LCM_SEND(6),{4,0,0xBD,0x00, 0x6c, 0x00}},

	{LCM_SEND(2),{0xBE,0x45}},//50

	{LCM_SEND(2),{0x35,0x00}},
	//{LCM_SEND(2),{0x36,0x08}},


	{LCM_SEND(2),{0x11,0x00}},

	{LCM_SLEEP(200)},

	{LCM_SEND(2),{0x29, 0x00}},

	{LCM_SLEEP(200)},

#else //old
	{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x03}},
{LCM_SEND(12),{10,0,0x90,0x03,0x14,0x01,0x00,0x00,0x00,0x38,0x00,0x00}},
{LCM_SEND(12),{10,0,0x91,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{LCM_SEND(14),{12,0,0x92,0x40,0x03,0x04,0x05,0x06,0x00,0x38,0x00,0x00,0x03,0x04}},
{LCM_SEND(11),{9,0,0x94,0x00,0x08,0x03,0x03,0xCA,0x03,0xCB,0x0C}},
{LCM_SEND(19),{17,0,0x95,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{LCM_SEND(5),{3,0,0x99,0x00,0x00}},
{LCM_SEND(14),{12,0,0x9A,0x00,0x0F,0x03,0xD4,0x03,0xD6,0x00,0x00,0x00,0x00,0x50}},
{LCM_SEND(9),{7,0,0x9B,0x01,0x38,0x00,0x00,0x00,0x00}},
{LCM_SEND(5),{3,0,0x9C,0x00,0x00}},
{LCM_SEND(11),{9,0,0x9D,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00}},
{LCM_SEND(5),{3,0,0x9E,0x00,0x00}},
{LCM_SEND(13),{11,0,0xA0,0x9F,0x1F,0x08,0x1F,0x0A,0x1F,0x00,0x1F,0x14,0x1F}},
{LCM_SEND(13),{11,0,0xA1,0x15,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F}},
{LCM_SEND(13),{11,0,0xA2,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F}},
{LCM_SEND(13),{11,0,0xA3,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F}},
{LCM_SEND(13),{11,0,0xA4,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F}},
{LCM_SEND(13),{11,0,0xA5,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x15}},
{LCM_SEND(13),{11,0,0xA6,0x1F,0x14,0x1F,0x01,0x1F,0x0B,0x1F,0x09,0x1F,0x1F}},
{LCM_SEND(13),{11,0,0xA7,0x1F,0x1F,0x0B,0x1F,0x09,0x1F,0x01,0x1F,0x15,0x1F}},
{LCM_SEND(13),{11,0,0xA8,0x14,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F}},
{LCM_SEND(13),{11,0,0xA9,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F}},
{LCM_SEND(13),{11,0,0xAA,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F}},
{LCM_SEND(13),{11,0,0xAB,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F}},
{LCM_SEND(13),{11,0,0xAC,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x14}},
{LCM_SEND(13),{11,0,0xAD,0x1F,0x15,0x1F,0x00,0x1F,0x08,0x1F,0x0A,0x1F,0x1F}},
{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x00}},
{LCM_SEND(2),{0xB1,0xFC}},
{LCM_SEND(6),{4,0,0xBC,0x00,0x00,0x00}},
{LCM_SEND(2),{0x4C,0x11}},
{LCM_SEND(7),{5,0,0xB8,0x01,0x8F,0xBF,0x8F}},
{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x01}},
{LCM_SEND(6),{4,0,0xB0,0x05,0x05,0x05}},
{LCM_SEND(6),{4,0,0xB1,0x05,0x05,0x05}},
{LCM_SEND(6),{4,0,0xB6,0x44,0x44,0x44}},
{LCM_SEND(6),{4,0,0xB7,0x34,0x34,0x34}},
{LCM_SEND(6),{4,0,0xB3,0x10,0x10,0x10}},
{LCM_SEND(6),{4,0,0xB9,0x24,0x24,0x24}},
{LCM_SEND(6),{4,0,0xB4,0x0A,0x0A,0x0A}},
{LCM_SEND(6),{4,0,0xBA,0x24,0x24,0x24}},
{LCM_SEND(6),{4,0,0xBC,0x00,0x70,0x00}},
{LCM_SEND(6),{4,0,0xBD,0x00,0x70,0x00}},
{LCM_SEND(2),{0xBE,0x50}},
{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x01}},
{LCM_SEND(19),{17,0,0xD1,0x00,0x00,0x00,0x12,0x00,0x2F,0x00,0x47,0x00,0x5D,0x00,0x7F,0x00,0x9D,0x00,0xCC}},
{LCM_SEND(19),{17,0,0xD2,0x00,0xF2,0x01,0x30,0x01,0x62,0x01,0xAF,0x01,0xEE,0x01,0xEF,0x02,0x28,0x02,0x63}},
{LCM_SEND(19),{17,0,0xD3,0x02,0x89,0x02,0xBB,0x02,0xDE,0x03,0x0D,0x03,0x2B,0x03,0x51,0x03,0x6A,0x03,0x87}},
{LCM_SEND(7),{5,0,0xD4,0x03,0xA5,0x03,0xFF}},
{LCM_SEND(19),{17,0,0xD5,0x00,0x00,0x00,0x12,0x00,0x2F,0x00,0x47,0x00,0x5D,0x00,0x7F,0x00,0x9D,0x00,0xCC}},
{LCM_SEND(19),{17,0,0xD6,0x00,0xF2,0x01,0x30,0x01,0x62,0x01,0xAF,0x01,0xEE,0x01,0xEF,0x02,0x28,0x02,0x63}},
{LCM_SEND(19),{17,0,0xD7,0x02,0x89,0x02,0xBB,0x02,0xDE,0x03,0x0D,0x03,0x2B,0x03,0x51,0x03,0x6A,0x03,0x87}},
{LCM_SEND(7),{5,0,0xD8,0x03,0xA5,0x03,0xFF}},
{LCM_SEND(19),{17,0,0xD9,0x00,0x00,0x00,0x12,0x00,0x2F,0x00,0x47,0x00,0x5D,0x00,0x7F,0x00,0x9D,0x00,0xCC}},
{LCM_SEND(19),{17,0,0xDD,0x00,0xF2,0x01,0x30,0x01,0x62,0x01,0xAF,0x01,0xEE,0x01,0xEF,0x02,0x28,0x02,0x63}},
{LCM_SEND(19),{17,0,0xDE,0x02,0x89,0x02,0xBB,0x02,0xDE,0x03,0x0D,0x03,0x2B,0x03,0x51,0x03,0x6A,0x03,0x87}},
{LCM_SEND(7),{5,0,0xDF,0x03,0xA5,0x03,0xFF}},
{LCM_SEND(19),{17,0,0xE0,0x00,0x00,0x00,0x12,0x00,0x2F,0x00,0x47,0x00,0x5D,0x00,0x7F,0x00,0x9D,0x00,0xCC}},
{LCM_SEND(19),{17,0,0xE1,0x00,0xF2,0x01,0x30,0x01,0x62,0x01,0xAF,0x01,0xEE,0x01,0xEF,0x02,0x28,0x02,0x63}},
{LCM_SEND(19),{17,0,0xE2,0x02,0x89,0x02,0xBB,0x02,0xDE,0x03,0x0D,0x03,0x2B,0x03,0x51,0x03,0x6A,0x03,0x87}},
{LCM_SEND(7),{5,0,0xE3,0x03,0xA5,0x03,0xFF}},
{LCM_SEND(19),{17,0,0xE4,0x00,0x00,0x00,0x12,0x00,0x2F,0x00,0x47,0x00,0x5D,0x00,0x7F,0x00,0x9D,0x00,0xCC}},
{LCM_SEND(19),{17,0,0xE5,0x00,0xF2,0x01,0x30,0x01,0x62,0x01,0xAF,0x01,0xEE,0x01,0xEF,0x02,0x28,0x02,0x63}},
{LCM_SEND(19),{17,0,0xE6,0x02,0x89,0x02,0xBB,0x02,0xDE,0x03,0x0D,0x03,0x2B,0x03,0x51,0x03,0x6A,0x03,0x87}},
{LCM_SEND(7),{5,0,0xE7,0x03,0xA5,0x03,0xFF}},
{LCM_SEND(19),{17,0,0xE8,0x00,0x00,0x00,0x12,0x00,0x2F,0x00,0x47,0x00,0x5D,0x00,0x7F,0x00,0x9D,0x00,0xCC}},
{LCM_SEND(19),{17,0,0xE9,0x00,0xF2,0x01,0x30,0x01,0x62,0x01,0xAF,0x01,0xEE,0x01,0xEF,0x02,0x28,0x02,0x63}},
{LCM_SEND(19),{17,0,0xEA,0x02,0x89,0x02,0xBB,0x02,0xDE,0x03,0x0D,0x03,0x2B,0x03,0x51,0x03,0x6A,0x03,0x87}},
{LCM_SEND(7),{5,0,0xEB,0x03,0xA5,0x03,0xFF}},
{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x02}},
{LCM_SEND(8),{6,0,0xC5,0x02,0x7C,0x00,0xFF,0x0A}},
{LCM_SEND(11),{9,0,0xC6,0x85,0xD6,0x8D,0x66,0x22,0x32,0xFF,0x0F}},
{LCM_SEND(20),{18,0,0xC7,0x10,0x52,0x97,0xFB,0x00,0x00,0xA0,0xBA,0x0B,0x00,0x60,0x00,0x88,0x88,0xF8,0xDB,0xFF}},
{LCM_SEND(10),{8,0,0xFE,0x00,0x10,0x00,0x94,0x05,0x00,0x4A}},
{LCM_SEND(7),{5,0,0xF3,0x10,0x5B,0x99,0x4F}},
{LCM_SEND(6),{4,0,0xF6,0xC0,0x90,0x60}},
{LCM_SEND(6),{4,0,0xF1,0x22,0x22,0x32}},
{LCM_SEND(8),{6,0,0xF0,0x55,0xAA,0x52,0x08,0x01}},
{LCM_SEND(2),{0x35,0x00}},
{LCM_SEND(2),{0x11,0x00}},
{LCM_SLEEP(120)},
{LCM_SEND(2),{0x29,0x00}},
{LCM_SLEEP(100)},                 	
#endif

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
static int32_t rm68191_mipi_init(struct panel_spec *self)
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

	pr_debug(KERN_DEBUG "rm68191_mipi_init\n");

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

static uint32_t rm68191_readid(struct panel_spec *self)
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

	printk("lcd_rm68191_mipi read id!\n");
	//return 0x8191;	//debug
#if 1
	mipi_set_cmd_mode();
	mipi_eotp_set(0,1);
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
		read_rtn = mipi_force_read(0xc5, 3,(uint8_t *)read_data);
		printk("lcd_rm68191_mipi read id 0xc5 value is 0x%x, 0x%x, 0x%x!\n", read_data[0], read_data[1], read_data[2]);

		if((0x81 == read_data[0])&&(0x91 == read_data[1])){
			printk("lcd_rm68191_mipi read id success!\n");
			mipi_eotp_set(1,1);
			return 0x8191;
		}
	}
	mipi_eotp_set(1,1);
	return 0x0;
#endif
}

static int32_t rm68191_enter_sleep(struct panel_spec *self, uint8_t is_sleep)
{
	int32_t i;
	LCM_Init_Code *sleep_in_out = NULL;
	unsigned int tag;
	int32_t size = 0;
	mipi_set_cmd_mode_t mipi_set_cmd_mode = self->info.mipi->ops->mipi_set_cmd_mode;
	mipi_gen_write_t mipi_gen_write = self->info.mipi->ops->mipi_gen_write;
	printk(KERN_DEBUG "rm68191_enter_sleep, is_sleep = %d\n", is_sleep);

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

static uint32_t rm68191_readpowermode(struct panel_spec *self)
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

	pr_debug("lcd_rm68191_mipi read power mode!\n");
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
		pr_debug("lcd_rm68191 mipi read power mode 0x0A value is 0x%x! , read result(%d)\n", read_data[0], read_rtn);
		if((0x9c == read_data[0])  && (0 == read_rtn)){
			pr_debug("lcd_rm68191_mipi read power mode success!\n");
			mipi_eotp_set(1,1);
			return 0x9c;
		}
	}

	printk("lcd_rm68191 mipi read power mode fail!0x0A value is 0x%x! , read result(%d)\n", read_data[0], read_rtn);
	mipi_eotp_set(1,1);
	return 0x0;
}

static int32_t rm68191_check_esd(struct panel_spec *self)
{
	uint32_t power_mode;
	//return 1;
	mipi_set_lp_mode_t mipi_set_data_lp_mode = self->info.mipi->ops->mipi_set_data_lp_mode;
	mipi_set_hs_mode_t mipi_set_data_hs_mode = self->info.mipi->ops->mipi_set_data_hs_mode;
	mipi_set_lp_mode_t mipi_set_lp_mode = self->info.mipi->ops->mipi_set_lp_mode;
	mipi_set_hs_mode_t mipi_set_hs_mode = self->info.mipi->ops->mipi_set_hs_mode;
	uint16_t work_mode = self->info.mipi->work_mode;
/*
	pr_debug("rm68191_check_esd!\n");
	if(SPRDFB_MIPI_MODE_CMD==work_mode){
		mipi_set_lp_mode();
	}else{
		mipi_set_data_lp_mode();
	}*/
	power_mode = rm68191_readpowermode(self);
	//power_mode = 0x0;
	/*
	if(SPRDFB_MIPI_MODE_CMD==work_mode){
		mipi_set_hs_mode();
	}else{
		mipi_set_data_hs_mode();
	}*/
	if(power_mode == 0x9c){
		printk("rm68191_check_esd OK!\n");
		return 1;
	}else{
		printk("rm68191_check_esd fail!(0x%x)\n", power_mode);
		return 0;
	}
}

static int32_t rm68191_after_suspend(struct panel_spec *self)
{
    // Do nothing;

    return 0;
}

static struct panel_operations lcd_rm68191_mipi_operations = {
	.panel_init = rm68191_mipi_init,
	.panel_readid = rm68191_readid,
	.panel_enter_sleep = rm68191_enter_sleep,
	.panel_esd_check = rm68191_check_esd,
    .panel_after_suspend = rm68191_after_suspend,
};

static struct timing_rgb lcd_rm68191_mipi_timing = {
	.hfp = 40,  /* 80unit: pixel */
	.hbp = 40, //80
	.hsync = 10,
	.vfp = 16, /*unit: line*/
	.vbp = 14,
	.vsync = 2,
};

static struct info_mipi lcd_rm68191_mipi_info = {
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
	.timing = &lcd_rm68191_mipi_timing,
	.ops = NULL,
};

struct panel_spec lcd_rm68191_mipi_spec = {
	.width = 540,
	.height = 960,
	.fps = 60,
	.type = LCD_MODE_DSI,
	.direction = LCD_DIRECT_NORMAL,
//	.is_clean_lcd = true,
	.info = {
		.mipi = &lcd_rm68191_mipi_info
	},
	.ops = &lcd_rm68191_mipi_operations,
	.suspend_mode = SEND_SLEEP_CMD,
};

struct panel_cfg lcd_rm68191_mipi = {
	/* this panel can only be main lcd */
	.dev_id = SPRDFB_MAINLCD_ID,
	.lcd_id = 0x8191,
	.lcd_name = "lcd_rm68191_mipi",
	.panel = &lcd_rm68191_mipi_spec,
};

static int __init lcd_rm68191_mipi_init(void)
{
	return sprdfb_panel_register(&lcd_rm68191_mipi);
}

subsys_initcall(lcd_rm68191_mipi_init);
