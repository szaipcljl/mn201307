/*
 *
 * FocalTech fts TouchScreen driver.
 * 
 * Copyright (c) 2010-2015, Focaltech Ltd. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

 /*******************************************************************************
*
* File Name: Focaltech_Gestrue.c
*
* Author: Xu YongFeng
*
* Created: 2015-01-29
*   
* Modify by mshl on 2015-10-26
*
* Abstract:
*
* Reference:
*
*******************************************************************************/

/*******************************************************************************
* 1.Included header files
*******************************************************************************/
#ifdef CONFIG_GST_TP_GESTURE_SUPPORT //huafeizhou141218 add

#include "focaltech_core.h"

extern u16 ft_gesture_enable;//huafeizhou160319 add
extern u8 ft_gesture_id;//huafeizhou160319 add
/*******************************************************************************
* Private constant and macro definitions using #define
*******************************************************************************/
#define  KEY_GESTURE_U			KEY_POWER//KEY_U
#define  KEY_GESTURE_UP		KEY_SCROLLLOCK//KEY_UP
#define  KEY_GESTURE_DOWN		KEY_F11//KEY_DOWN
#define  KEY_GESTURE_LEFT		KEY_F9//KEY_LEFT 
#define  KEY_GESTURE_RIGHT		KEY_F10//KEY_RIGHT
#define  KEY_GESTURE_O			KEY_F1//KEY_O
#define  KEY_GESTURE_E			KEY_F4//KEY_E
#define  KEY_GESTURE_C			KEY_F5//KEY_C
#define  KEY_GESTURE_M			KEY_F3//KEY_M 
#define  KEY_GESTURE_L			KEY_L//KEY_L
#define  KEY_GESTURE_W			KEY_F2//KEY_W
#define  KEY_GESTURE_S			KEY_F6//KEY_S 
#define  KEY_GESTURE_V			KEY_F7//KEY_V
#define  KEY_GESTURE_Z			KEY_F8//KEY_Z

#define GESTURE_LEFT		0x20
#define GESTURE_RIGHT		0x21
#define GESTURE_UP		    0x22
#define GESTURE_DOWN		0x23
#define GESTURE_DOUBLECLICK	0x24
#define GESTURE_O		    0x30
#define GESTURE_W		    0x31
#define GESTURE_M		    0x32
#define GESTURE_E		    0x33
#define GESTURE_C		    0x34
#define GESTURE_a		    0x36  //huafeizhou150206 add  /*used for a,not support A*/
#define GESTURE_L		    0x44
#define GESTURE_S		    0x46
#define GESTURE_v		    0x53  //huafeizhou150206 add  /*used for ^*/
#define GESTURE_V		    0x54
#define GESTURE_Z		    0x65
#define FTS_GESTRUE_POINTS 255
#define FTS_GESTRUE_POINTS_ONETIME  62
#define FTS_GESTRUE_POINTS_HEADER 8
#define FTS_GESTURE_OUTPUT_ADRESS 0xD3
#define FTS_GESTURE_OUTPUT_UNIT_LENGTH 4

/*******************************************************************************
* Private enumerations, structures and unions using typedef
*******************************************************************************/


/*******************************************************************************
* Static variables
*******************************************************************************/
short pointnum = 0;
unsigned short coordinate_x[150] = {0};
unsigned short coordinate_y[150] = {0};

/*******************************************************************************
* Global variable or extern global variabls/functions
*******************************************************************************/

/*******************************************************************************
* Static function prototypes
*******************************************************************************/


/*******************************************************************************
* Name: fts_Gesture_init
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
int fts_Gesture_init(struct input_dev *input_dev)
{
	input_set_capability(input_dev, EV_KEY, KEY_POWER);
	input_set_capability(input_dev, EV_KEY, KEY_GESTURE_U); 
	input_set_capability(input_dev, EV_KEY, KEY_GESTURE_UP); 
	input_set_capability(input_dev, EV_KEY, KEY_GESTURE_DOWN);
	input_set_capability(input_dev, EV_KEY, KEY_GESTURE_LEFT); 
	input_set_capability(input_dev, EV_KEY, KEY_GESTURE_RIGHT); 
	input_set_capability(input_dev, EV_KEY, KEY_GESTURE_O);
	input_set_capability(input_dev, EV_KEY, KEY_GESTURE_E); 
	input_set_capability(input_dev, EV_KEY, KEY_GESTURE_C); 
	input_set_capability(input_dev, EV_KEY, KEY_GESTURE_M); 
	input_set_capability(input_dev, EV_KEY, KEY_GESTURE_L);
	input_set_capability(input_dev, EV_KEY, KEY_GESTURE_W);
	input_set_capability(input_dev, EV_KEY, KEY_GESTURE_S); 
	input_set_capability(input_dev, EV_KEY, KEY_GESTURE_V);
	input_set_capability(input_dev, EV_KEY, KEY_GESTURE_Z);
		
	__set_bit(KEY_GESTURE_RIGHT, input_dev->keybit);
	__set_bit(KEY_GESTURE_LEFT, input_dev->keybit);
	__set_bit(KEY_GESTURE_UP, input_dev->keybit);
	__set_bit(KEY_GESTURE_DOWN, input_dev->keybit);
	__set_bit(KEY_GESTURE_U, input_dev->keybit);
	__set_bit(KEY_GESTURE_O, input_dev->keybit);
	__set_bit(KEY_GESTURE_E, input_dev->keybit);
	__set_bit(KEY_GESTURE_C, input_dev->keybit);
	__set_bit(KEY_GESTURE_M, input_dev->keybit);
	__set_bit(KEY_GESTURE_W, input_dev->keybit);
	__set_bit(KEY_GESTURE_L, input_dev->keybit);
	__set_bit(KEY_GESTURE_S, input_dev->keybit);
	__set_bit(KEY_GESTURE_V, input_dev->keybit);
	__set_bit(KEY_GESTURE_Z, input_dev->keybit);

	return 0;
}

/*******************************************************************************
* Name: fts_check_gesture
* Brief:
* Input:
* Output: None
* Return: None
*******************************************************************************/
static void fts_check_gesture(struct input_dev *input_dev,int gesture_id)
{
	printk("****fts_check_gesture( gesture_id==0x%x)\n ",gesture_id);

	ft_gesture_id=gesture_id;//huafeizhou141220 add
	switch(gesture_id)
	{
	        case GESTURE_LEFT:
				if((ft_gesture_enable & TP_GESTURE_LEFT)==TP_GESTURE_LEFT)
				{
	                input_report_key(input_dev, KEY_GESTURE_LEFT, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_GESTURE_LEFT, 0);
	                input_sync(input_dev);
				}
	            break;
	        case GESTURE_RIGHT:
				if((ft_gesture_enable & TP_GESTURE_RIGHT)==TP_GESTURE_RIGHT)
				{
	                input_report_key(input_dev, KEY_GESTURE_RIGHT, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_GESTURE_RIGHT, 0);
	                input_sync(input_dev);
				}
			    break;
	        case GESTURE_UP:
				if((ft_gesture_enable & TP_GESTURE_UP)==TP_GESTURE_UP)
				{			
	                input_report_key(input_dev, KEY_GESTURE_UP, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_GESTURE_UP, 0);
	                input_sync(input_dev);  
				}                                
	            break;
	        case GESTURE_DOWN:
				if((ft_gesture_enable & TP_GESTURE_DOWN)==TP_GESTURE_DOWN)
				{			
	                input_report_key(input_dev, KEY_GESTURE_DOWN, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_GESTURE_DOWN, 0);
	                input_sync(input_dev);
				}
	            break;
	        case GESTURE_DOUBLECLICK:
				//if((ft_gesture_enable & TP_GESTURE_DOUBLECLICK)==TP_GESTURE_DOUBLECLICK)
				{
	                input_report_key(input_dev, KEY_GESTURE_U, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_GESTURE_U, 0);
	                input_sync(input_dev);
				}
	            break;
	        case GESTURE_O:
				if((ft_gesture_enable & TP_GESTURE_O)==TP_GESTURE_O)
				{			
	                input_report_key(input_dev, KEY_GESTURE_O, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_GESTURE_O, 0);
	                input_sync(input_dev);
				}
	            break;
	        case GESTURE_W:
				if((ft_gesture_enable & TP_GESTURE_W)==TP_GESTURE_W)
				{			
	                input_report_key(input_dev, KEY_GESTURE_W, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_GESTURE_W, 0);
	                input_sync(input_dev);
				}
	            break;
	        case GESTURE_M:
				if((ft_gesture_enable & TP_GESTURE_M)==TP_GESTURE_M)
				{			
	                input_report_key(input_dev, KEY_GESTURE_M, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_GESTURE_M, 0);
	                input_sync(input_dev);
				}
	            break;
	        case GESTURE_E:
				if((ft_gesture_enable & TP_GESTURE_E)==TP_GESTURE_E)
				{
	                input_report_key(input_dev, KEY_GESTURE_E, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_GESTURE_E, 0);
	                input_sync(input_dev);
				}
	            break;
	        case GESTURE_C:
				if((ft_gesture_enable & TP_GESTURE_C)==TP_GESTURE_C)
				{
	                input_report_key(input_dev, KEY_GESTURE_C, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_GESTURE_C, 0);
	                input_sync(input_dev);
				}
	            break;
	        case GESTURE_L:
				if((ft_gesture_enable & TP_GESTURE_L)==TP_GESTURE_L)
				{
	                input_report_key(input_dev, KEY_GESTURE_L, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_GESTURE_L, 0);
	                input_sync(input_dev);
				}
	            break;
	        case GESTURE_S:
				if((ft_gesture_enable & TP_GESTURE_S)==TP_GESTURE_S)
				{
	                input_report_key(input_dev, KEY_GESTURE_S, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_GESTURE_S, 0);
	                input_sync(input_dev);
				}
	            break;
	        case GESTURE_V:
				if((ft_gesture_enable & TP_GESTURE_V)==TP_GESTURE_V)
				{
	                input_report_key(input_dev, KEY_GESTURE_V, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_GESTURE_V, 0);
	                input_sync(input_dev);
				}
	            break;
	        case GESTURE_Z:
				if((ft_gesture_enable & TP_GESTURE_Z)==TP_GESTURE_Z)
				{
	                input_report_key(input_dev, KEY_GESTURE_Z, 1);
	                input_sync(input_dev);
	                input_report_key(input_dev, KEY_GESTURE_Z, 0);
	                input_sync(input_dev);
				}
	            break;
	        default:
	                break;
	}

}

 /************************************************************************
* Name: fts_read_Gestruedata
* Brief: read data from TP register
* Input: no
* Output: no
* Return: fail <0
***********************************************************************/
int fts_read_Gestruedata(void)
{
	unsigned char buf[FTS_GESTRUE_POINTS * 3] = { 0 };
	int ret = -1;
	int i = 0;
	int gestrue_id = 0;

	buf[0] = 0xd3;
	pointnum = 0;

	ret = fts_i2c_read(fts_i2c_client, buf, 1, buf, FTS_GESTRUE_POINTS_HEADER);

	if (ret < 0)
	{
		printk( "%s read touchdata failed.\n", __func__);
		return ret;
	}

	 if (fts_updateinfo_curr.CHIP_ID == 0x64||fts_updateinfo_curr.CHIP_ID == 0x54 || fts_updateinfo_curr.CHIP_ID == 0x58 || fts_updateinfo_curr.CHIP_ID == 0x86 || fts_updateinfo_curr.CHIP_ID == 0x87)
	 {
		 gestrue_id = buf[0];

		 if(fts_updateinfo_curr.CHIP_ID!=0x64)
		 {
		 pointnum = (short)(buf[1]) & 0xff;
		 buf[0] = 0xd3;

		 if((pointnum * 4 + 2)<255)
		 {
		    	 ret = fts_i2c_read(fts_i2c_client, buf, 1, buf, (pointnum * 4 + 2));
		 }
		 else
		 {
		        ret = fts_i2c_read(fts_i2c_client, buf, 1, buf, 255);
		        ret = fts_i2c_read(fts_i2c_client, buf, 0, buf+255, (pointnum * 4 + 2) -255);
		 }
		 if (ret < 0)
		 {
		       printk( "%s read touchdata failed.\n", __func__);
		       return ret;
		 }
		 }
		 fts_check_gesture(fts_input_dev,gestrue_id);
		 for(i = 0;i < pointnum;i++)
		 {
		    	coordinate_x[i] =  (((s16) buf[2 + (4 * i)]) & 0x0F) <<
		        	8 | (((s16) buf[3 + (4 * i)])& 0xFF);
		    	coordinate_y[i] = (((s16) buf[4 + (4 * i)]) & 0x0F) <<
		        	8 | (((s16) buf[5 + (4 * i)]) & 0xFF);
		 }
		 return -1;
	}
	else
	{
		if (0x24 == buf[0])
		{
			gestrue_id = 0x24;
			fts_check_gesture(fts_input_dev,gestrue_id);
			printk( "check_gesture gestrue_id=0x%x.\n", gestrue_id);
			return -1;
		}

		pointnum = (short)(buf[1]) & 0xff;
		buf[0] = 0xd3;
		if((pointnum * 4 + 8)<255)
		{
			ret = fts_i2c_read(fts_i2c_client, buf, 1, buf, (pointnum * 4 + 8));
		}
		else
		{
			ret = fts_i2c_read(fts_i2c_client, buf, 1, buf, 255);
			ret = fts_i2c_read(fts_i2c_client, buf, 0, buf+255, (pointnum * 4 + 8) -255);
		}
		if (ret < 0)
		{
			printk( "%s read touchdata failed.\n", __func__);
			return ret;
		}

		gestrue_id = fetch_object_sample(buf, pointnum);
		fts_check_gesture(fts_input_dev,gestrue_id);
		printk( "%d read gestrue_id.\n", gestrue_id);

		for(i = 0;i < pointnum;i++)
		{
		    coordinate_x[i] =  (((s16) buf[0 + (4 * i)]) & 0x0F) <<
		        8 | (((s16) buf[1 + (4 * i)])& 0xFF);
		    coordinate_y[i] = (((s16) buf[2 + (4 * i)]) & 0x0F) <<
		        8 | (((s16) buf[3 + (4 * i)]) & 0xFF);
		}
		return -1;
	}
}
#endif /*FTS_GESTRUE_EN */ //huafeizhou160319 add
