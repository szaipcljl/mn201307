/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "sensor_cfg.h"
#include "sensor_drv_u.h"

//#define  SP0A20_TCARD_TEST
#ifdef SP0A20_TCARD_TEST
#include <fcntl.h>              /* low-level i/o */
#include <errno.h>
#include <sys/ioctl.h>

#include "sensor_cfg.h"
#include "sensor_drv_u.h"
#include "cmr_msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
 //void    _sp0A20_sdcard(char *);
//#define SENSOR_TRACE printk
static int num_sd; 
#endif

/**---------------------------------------------------------------------------*
 ** 						   Macro Define
 **---------------------------------------------------------------------------*/
#ifdef SENSOR_PRINT
#undef SENSOR_PRINT
#endif
#define SENSOR_PRINT	CMR_LOGE

#define sp0a20_I2C_ADDR_W	0x21 // 0x42 --> 0x42 / 2
#define sp0a20_I2C_ADDR_R	0x21 // 0x43 --> 0x43 / 2

#define SENSOR_GAIN_SCALE		16
/**---------------------------------------------------------------------------*
 ** 					Local Function Prototypes							  *
 **---------------------------------------------------------------------------*/


/**---------------------------------------------------------------------------*
 ** 						Local Variables 								 *
 **---------------------------------------------------------------------------*/
LOCAL SENSOR_IOCTL_FUNC_TAB_T s_sp0a20_ioctl_func_tab;

typedef enum
{
	FLICKER_50HZ = 0,
	FLICKER_60HZ,
	FLICKER_MAX
}FLICKER_E;
LOCAL uint32_t		Antiflicker		 = FLICKER_50HZ;
//heq
  #define SP0A20_P1_0x10		0x80		 //ku_outdoor
  #define SP0A20_P1_0x11		0x80		//ku_nr
  #define SP0A20_P1_0x12		0x80		 //ku_dummy
  #define SP0A20_P1_0x13		0x80		 //ku_low
  
  #define SP0A20_P1_0x14		0x98		//c4 //kl_outdoor 
  #define SP0A20_P1_0x15		0x98		//c4 //kl_nr      
  #define SP0A20_P1_0x16		0x98		//c4 //kl_dummy    
  #define SP0A20_P1_0x17		0x98		//c4 //kl_low   
#ifdef SP0A20_TCARD_TEST
SENSOR_REG_T SP0A20_YUV_640X480[1000]=
#else
SENSOR_REG_T SP0A20_YUV_640X480[]=
#endif
{
	{0xfd,0x00},
	{0x0c,0x00},//mipi ldo power on
	{0x96,0x03},
	{0x12,0x02},
	{0x13,0x2f},
	{0x6d,0x32},
	{0x6c,0x32},
	{0x6f,0x33},
	{0x6e,0x34},
	{0x16,0x38},
	{0x17,0x38},
	{0x70,0x3a},
	{0x14,0x02},
	{0x15,0x20},
	{0x71,0x23},
	{0x69,0x25},
	{0x6a,0x1a},
	{0x72,0x1c},
	{0x75,0x1e},
	{0x73,0x3c},
	{0x74,0x21},
	{0x79,0x00},
	{0x77,0x10},
	{0x1a,0x4d},
	{0x1e,0x15},
	{0x21,0x08},//
	{0x22,0x28},
	{0x26,0x66},
	{0x28,0x0b},
	{0x37,0x4a},
	{0xfd,0x02},
	{0x01,0x80},
	{0x52,0x10},
	{0x54,0x00},
	{0xfd,0x01},
	{0x41,0x00},
	{0x42,0x00},
	{0x43,0x00},
	{0x44,0x00},
	//24M 50Hz  7-15fps
	{0xfd,0x00},
	{0x03,0x01},
	{0x04,0xc2},
	{0x05,0x00},
	{0x06,0x00},
	{0x07,0x00},
	{0x08,0x00},
	{0x09,0x02},
	{0x0a,0xf4},
	{0xfd,0x01},
	{0xf0,0x00},
	{0xf7,0x4b},
	{0x02,0x0e},
	{0x03,0x01},
	{0x06,0x4b},
	{0x07,0x00},
	{0x08,0x01},
	{0x09,0x00},
	{0xfd,0x02},
	{0xbe,0x1a},
	{0xbf,0x04},
	{0xd0,0x1a},
	{0xd1,0x04},

	{0xfd,0x01},
	{0x5a,0x40},
	{0xfd,0x02},
	{0xbc,0x70},
	{0xbd,0x50},
	{0xb8,0x66},
	{0xb9,0x80},
	{0xba,0x30},
	{0xbb,0x45},
	{0xfd,0x01},
	{0xe0,0x50},
	{0xe1,0x3e},
	{0xe2,0x36},
	{0xe3,0x30},
	{0xe4,0x30},
	{0xe5,0x2e},
	{0xe6,0x2e},
	{0xe7,0x2c},
	{0xe8,0x2c},
	{0xe9,0x2c},
	{0xea,0x2a},
	{0xf3,0x2a},
	{0xf4,0x2a},
	{0xfd,0x01},
	{0x04,0xa0},
	{0x05,0x24},
	{0x0a,0xa0},
	{0x0b,0x24},
	{0xfd,0x01},
	{0xeb,0x78},
	{0xec,0x78},
	{0xed,0x05},
	{0xee,0x0a},
	{0xfd,0x01},
	{0xf2,0x4d},
	{0xfd,0x02},
	{0x5b,0x05},
	{0x5c,0xa0},
	{0xfd,0x01}, //page 1
	{0x26,0x80},
	{0x27,0x4f},
	{0x28,0x00},
	{0x29,0x20},
	{0x2a,0x00},
	{0x2b,0x03},
	{0x2c,0x00},
	{0x2d,0x20},
	{0x30,0x00},
	{0x31,0x00}, //page 1 is not mirror 
	{0xfd,0x01},
	{0xa1,0x13},
	{0xa2,0x13},
	{0xa3,0x11},
	{0xa4,0x13},
	{0xa5,0x0c},
	{0xa6,0x0c},
	{0xa7,0x0c},
	{0xa8,0x0c},
	{0xa9,0x0a},
	{0xaa,0x0a},
	{0xab,0x0a},
	{0xac,0x0a},
	{0xad,0x00},
	{0xae,0x00},
	{0xaf,0x00},
	{0xb0,0x00},
	{0xb1,0x00},
	{0xb2,0x00},
	{0xb3,0x00},
	{0xb4,0x00},
	{0xb5,0x00},
	{0xb6,0x00},
	{0xb7,0x00},
	{0xb8,0x00},
	{0xfd,0x02},
	{0x08,0x00},
	{0x09,0x06},
	{0x1d,0x03},
	{0x1f,0x05},
	{0xfd,0x01},
	{0x32,0x00},
	{0xfd,0x02},
	{0x26,0xa0},
	{0x27,0xa8},
	{0x10,0x00},
	{0x11,0x00},
	{0x1b,0x80},
	{0x1a,0x80},
	{0x18,0x27},
	{0x19,0x26},
	{0x2a,0x00},
	{0x2b,0x00},
	{0x28,0xf8},
	{0x29,0x08},
	{0x66,0x4b},
	{0x67,0x46},
	{0x68,0xcd},
	{0x69,0xe7},
	{0x6a,0xa6},
	{0x7c,0x3c},
	{0x7d,0x5c},
	{0x7e,0xf6},
	{0x7f,0x1b},
	{0x80,0xa6},
	{0x70,0x2a},
	{0x71,0x4a},
	{0x72,0x30},
	{0x73,0x50},
	{0x74,0xaa},
	{0x6b,0x10},
	{0x6c,0x30},
	{0x6d,0x30},
	{0x6e,0x50},
	{0x6f,0xaa},
	{0x61,0x04},
	{0x62,0x20},
	{0x63,0x38},
	{0x64,0x60},
	{0x65,0x6a},
	{0x75,0x80},
	{0x76,0x09},
	{0x77,0x02},
	{0x24,0x25},
	{0x0e,0x16},
	{0x3b,0x09},
	{0xfd,0x02},
	{0xde,0x0f},
	{0xd7,0x08},
	{0xd8,0x08},
	{0xd9,0x10},
	{0xda,0x14},
	{0xe8,0x20},
	{0xe9,0x20},
	{0xea,0x18},
	{0xeb,0x18},
	{0xec,0x24},
	{0xed,0x24},
	{0xee,0x20},
	{0xef,0x20},
	{0xd3,0x20},
	{0xd4,0x30},
	{0xd5,0x08},
	{0xd6,0x08},
	{0xfd,0x01},
	{0xd1,0x20},
	{0xfd,0x02},
	{0xdc,0x05},
	{0x05,0x20},
	{0xfd,0x02},
	{0x81,0x00},
	{0xfd,0x01},
	{0xfc,0x00},
	{0x7d,0x05},
	{0x7e,0x05},
	{0x7f,0x09},
	{0x80,0x08},
	{0xfd,0x02},
	{0xdd,0x0f},
	{0xfd,0x01},
	{0x6d,0x08},
	{0x6e,0x08},
	{0x6f,0x10},
	{0x70,0x14},
	{0x86,0x14},
	{0x71,0x08},
	{0x72,0x08},
	{0x73,0x10},
	{0x74,0x14},
	{0x75,0x08},
	{0x76,0x08},
	{0x77,0x06},
	{0x78,0x06},
	{0x79,0x77},
	{0x7a,0x77},
	{0x7b,0x55},
	{0x7c,0x22},
	{0x81,0x0d},
	{0x82,0x18},
	{0x83,0x20},
	{0x84,0x24},
	{0xfd,0x02},
	{0x83,0x12},
	{0x84,0x14},
	{0x86,0x04},
	{0xfd,0x01},
	{0x61,0x60},
	{0x62,0x28},
	{0x8a,0x10},
	{0xfd,0x01},
	{0x8b,0x00},
	{0x8c,0x0a},
	{0x8d,0x18},
	{0x8e,0x29},
	{0x8f,0x39},
	{0x90,0x4f},
	{0x91,0x62},
	{0x92,0x71},
	{0x93,0x7f},
	{0x94,0x93},
	{0x95,0xa3},
	{0x96,0xb0},
	{0x97,0xbd},
	{0x98,0xc8},
	{0x99,0xd1},
	{0x9a,0xd9},
	{0x9b,0xe1},
	{0x9c,0xe8},
	{0x9d,0xee},
	{0x9e,0xf4},
	{0x9f,0xfa},
	{0xa0,0xff},
	{0xfd,0x02},
	{0x15,0xc0},
	{0x16,0x8c},
	{0xa0,0x93},
	{0xa1,0xe7},
	{0xa2,0x06},
	{0xa3,0x06},
	{0xa4,0x91},
	{0xa5,0xe8},
	{0xa6,0x06},
	{0xa7,0xe8},
	{0xa8,0x91},
	{0xa9,0x0c},
	{0xaa,0x30},
	{0xab,0x0c},
	{0xac,0x93},
	{0xad,0xe7},
	{0xae,0x06},
	{0xaf,0x06},
	{0xb0,0x91},
	{0xb1,0xe8},
	{0xb2,0x06},
	{0xb3,0xe8},
	{0xb4,0x91},
	{0xb5,0x0c},
	{0xb6,0x30},
	{0xb7,0x0c},
	{0xfd,0x01},
	{0xd3,0x80},
	{0xd4,0x80},
	{0xd5,0x70},
	{0xd6,0x60},
	{0xd7,0x80},
	{0xd8,0x80},
	{0xd9,0x70},
	{0xda,0x60},
	{0xfd,0x01},
	{0xdd,0x30},
	{0xde,0x10},
	{0xdf,0xff},
	{0x00,0x00},
	{0xfd,0x01},
	{0xc2,0xaa},
	{0xc3,0x88},
	{0xc4,0x77},
	{0xc5,0x66},
	{0xfd,0x01},
	{0xcd,0x10},
	{0xce,0x1f},
	{0xcf,0x30},
	{0xd0,0x45},
	{0xfd,0x02},//page 2
	{0x31,0x60},
	{0x32,0x60},
	{0x33,0xc0},
	{0x35,0x60},
	{0x37,0x13},
	{0xfd,0x01},
	{0x0e,0x80},
	{0x0f,0x20},
	{0x10,0x80},
	{0x11,0x80},
	{0x12,0x80},
	{0x13,0x80},
	{0x14,0x98},
	{0x15,0x98},
	{0x16,0x98},
	{0x17,0x98},
	{0xfd,0x00}, //page 0
	{0x31,0x06},//mirror 0x00 bit1,bit2
	{0xfd,0x01},
	{0x32,0x15},
	{0x33,0xef},
	{0x34,0x07},
	{0xd2,0x01},
	{0xfb,0x25},
	{0xf2,0x49},
	{0x35,0x40},
	{0x5d,0x11},
	{0xfd,0x00},
	//{0x98,0x44},
	{0x99,0x04},//open driver
	{0xfd,0x00},
};
#ifdef SP0A20_TCARD_TEST   
	#define SP0A20_OP_CODE_INI		0x00		/* Initial value. */
	#define SP0A20_OP_CODE_REG		0x01		/* Register */
	#define SP0A20_OP_CODE_DLY		0x02		/* Delay */
	#define SP0A20_OP_CODE_END		0x03		/* End of initial setting. */
	

	#define u16  unsigned short
	#define u8   unsigned char
	#define u32  unsigned int
		typedef struct
	{
		u16 init_reg;
		u16 init_val;	/* Save the register value and delay tick */
		u8 op_code;		/* 0 - Initial value, 1 - Register, 2 - Delay, 3 - End of setting. */
	} SP0A20_initial_set_struct;

	SP0A20_initial_set_struct SP0A20_Init_Reg[500];




unsigned char data_buff[10*1024]; 
unsigned char *curr_ptr ;	
FILE*fp;		
int file_size;
int flag_sd =1;



 u32 sp_strtol(const char *nptr, u8 base)
{
	u32 ret;
	
	if(!nptr || (base!=16 && base!=10 && base!=8))
	{
		SENSOR_TRACE("%s(): NULL pointer input\n", __FUNCTION__);
		return -1;
	}
	for(ret=0; *nptr; nptr++)
	{
		if((base==16 && *nptr>='A' && *nptr<='F') || 
			(base==16 && *nptr>='a' && *nptr<='f') || 
			(base>=10 && *nptr>='0' && *nptr<='9') ||
			(base>=8 && *nptr>='0' && *nptr<='7') )
		{
			ret *= base;
			if(base==16 && *nptr>='A' && *nptr<='F')
				ret += *nptr-'A'+10;
			else if(base==16 && *nptr>='a' && *nptr<='f')
				ret += *nptr-'a'+10;
			else if(base>=10 && *nptr>='0' && *nptr<='9')
				ret += *nptr-'0';
			else if(base>=8 && *nptr>='0' && *nptr<='7')
				ret += *nptr-'0';
		}
		else
			return ret;
	}
	
	return ret;
}



#if 1
unsigned char SP0A20_Initialize_from_T_Flash()
{
	u8 func_ind[4] = {0};
	int i=0;

	SENSOR_TRACE("hello:SP0A20_Initialize_from_T_Flash(start)\n");
	
	curr_ptr = data_buff;
	while (curr_ptr < (data_buff + file_size))
	{
		while ((*curr_ptr == ' ') || (*curr_ptr == '\t'))/* Skip the Space & TAB */
			curr_ptr++;				

		if (((*curr_ptr) == '/') && ((*(curr_ptr + 1)) == '*'))
		{
			while (!(((*curr_ptr) == '*') && ((*(curr_ptr + 1)) == '/')))
			{
				curr_ptr++;		/* Skip block comment code. */
			}

			while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
			{
				curr_ptr++;
			}

			curr_ptr += 2;						/* Skip the enter line */
			
			continue ;
		}
		
		if (((*curr_ptr) == '/') || ((*curr_ptr) == '{') || ((*curr_ptr) == '}'))		/* Comment line, skip it. */
		{
			while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
			{
				curr_ptr++;
			}

			curr_ptr += 2;						/* Skip the enter line */

			continue ;
		}
		/* This just content one enter line. */
		if (((*curr_ptr) == 0x0D) && ((*(curr_ptr + 1)) == 0x0A))
		{
			curr_ptr += 2;
			continue ;
		}
		//printk(" curr_ptr1 = %s\n",curr_ptr);
		memcpy(func_ind, curr_ptr, 3);
	        func_ind[3] = '\0';
						
		if (strcmp((const char *)func_ind, "REG") == 0)		/* REG */
		{
			curr_ptr += 6;				/* Skip "REG(0x" or "DLY(" */
			SP0A20_Init_Reg[i].op_code = SP0A20_OP_CODE_REG;
			
			SP0A20_Init_Reg[i].init_reg = sp_strtol((const char *)curr_ptr, 16);
			curr_ptr += 5;	/* Skip "00, 0x" */
		
			SP0A20_Init_Reg[i].init_val = sp_strtol((const char *)curr_ptr, 16);
			curr_ptr += 4;	/* Skip "00);" */
		
		}
		else									/* DLY */
		{
			#if 0 //lj_test
			/* Need add delay for this setting. */
			curr_ptr += 4;	
			SP0A20_Init_Reg[i].op_code = SP0A20_OP_CODE_DLY;

			
			SP0A20_Init_Reg[i].init_reg = 0xFF;
			SP0A20_Init_Reg[i].init_val = SP0A20_strtol((const char *)curr_ptr,  10);	/* Get the delay ticks, the delay should less then 50 */
			#endif
		}
		i++;
		num_sd =i;
#if 0 //lj_test
		/* Skip to next line directly. */

		while (!((*curr_ptr == 0x0D) && (*(curr_ptr+1) == 0x0A)))
		{
			curr_ptr++;
		}

		curr_ptr += 2;
#endif
	}

	SENSOR_TRACE("hello:SP0A20_Initialize_from_T_Flash(end)\n");

return 1;
	
}
#endif


  void    _sp0A20_sdcard_reg(void)
{

	int i;

	SENSOR_TRACE("hello:_sp0A20_sdcard_reg(start)\n");
	
	for(i = 0;i<num_sd;i++)
	{
		SP0A20_YUV_640X480[i].reg_addr = SP0A20_Init_Reg[i].init_reg;
		SP0A20_YUV_640X480[i].reg_value=SP0A20_Init_Reg[i].init_val;
		//SENSOR_TRACE("gpwreg11 %x = %x\n",SP0A20_YUV_COMMON[i].reg_addr ,SP0A20_YUV_COMMON[i].reg_value);
		//CMR_LOGE("gpwreg11 %x = %x\n",SP0A20_YUV_COMMON[i].reg_addr ,SP0A20_YUV_COMMON[i].reg_value);
		//CMR_LOGE("gpwreg11 %x = %x\n",SP0A20_Init_Reg[i].reg_addr ,SP0A20_Init_Reg[i].reg_value);
		
	}

	if(num_sd  != 0)
	{
		for(i =num_sd;i<500;i++)
		{
			SP0A20_YUV_640X480[i].reg_addr = 0xfd;
			SP0A20_YUV_640X480[i].reg_value= 0x00;
			//SENSOR_TRACE("gpwreg %x = %x\n",SP0A20_YUV_COMMON[i].reg_addr ,SP0A20_YUV_COMMON[i].reg_value);
		}
	}

	SENSOR_TRACE("hello:_sp0A20_sdcard_reg(end)\n");
}


 void    _sp0A20_sdcard(void)
{
	
	 int i;
	 int cnt;
	 
	SENSOR_TRACE("hello:_sp0A20_sdcard(start)\n");
	
	memset(data_buff,0,sizeof(data_buff));//before use the buffer,clean
	fp = fopen("/system/lib/sp0a20_sd", "r");

	
	if(NULL == fp){  		
		SENSOR_TRACE("open file error\n");	
		//_sp0A20_no_sdcard();//if no sdcard ,of open file error,use the origianl para
	} 	
	else 
	{
	fseek(fp, 0, SEEK_END);

	file_size = ftell(fp);

	rewind(fp);

	cnt = (int)fread(data_buff, 1, file_size , fp);
	
	SENSOR_TRACE("open file ok %d\n" ,file_size);
	fclose(fp); 	
	
	SP0A20_Initialize_from_T_Flash();//Analysis parameters

	_sp0A20_sdcard_reg();//copy para and fill para
 
	}

	SENSOR_TRACE("hello:_sp0A20_sdcard(end)\n");
}

#endif

LOCAL SENSOR_REG_TAB_INFO_T s_sp0a20_resolution_Tab_YUV[]=
{
	// COMMON INIT
	{ADDR_AND_LEN_OF_ARRAY(SP0A20_YUV_640X480), 640, 480, 24, SENSOR_IMAGE_FORMAT_YUV422},
	//{PNULL, 0, 0, 0, 24, SENSOR_IMAGE_FORMAT_YUV422},

	// YUV422 PREVIEW 1
	{ADDR_AND_LEN_OF_ARRAY(SP0A20_YUV_640X480), 640, 480, 24, SENSOR_IMAGE_FORMAT_YUV422},
	//{PNULL, 0, 640, 480, 24, SENSOR_IMAGE_FORMAT_YUV422},
	{PNULL, 0, 0, 0, 0, 0},
	{PNULL, 0, 0, 0, 0, 0},
	{PNULL, 0, 0, 0, 0, 0},

	// YUV422 PREVIEW 2
	{ PNULL,                    0,      0,  0  ,        0,        0      },
	{ PNULL,                    0,      0,  0  ,        0,        0      },
	{ PNULL,                    0,      0,  0  ,        0,        0      },
	{ PNULL,                    0,      0,  0  ,        0,        0      }
};

LOCAL SENSOR_TRIM_T s_sp0a20_Resolution_Trim_Tab[] = {
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 640, 480, 261, 340, 480, {0, 0, 640, 480}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
};

LOCAL uint32_t sp0a20_GetResolutionTrimTab(uint32_t param)
{
	SENSOR_PRINT("0x%x", (uint32_t)s_sp0a20_Resolution_Trim_Tab);
	return (uint32_t) s_sp0a20_Resolution_Trim_Tab;
}

SENSOR_INFO_T g_SP0A20_yuv_info =
{
	sp0a20_I2C_ADDR_W,				// salve i2c write address
	sp0a20_I2C_ADDR_R, 				// salve i2c read address

	0, //SENSOR_I2C_VAL_8BIT | SENSOR_I2C_REG_8BIT | SENSOR_I2C_FREQ_200,//	0,								// bit0: 0: i2c register value is 8 bit, 1: i2c register value is 16 bit
	//bit2: 0: i2c register addr  is 8 bit, 1: i2c register addr  is 16 bit
	//other bit: reseved
	SENSOR_HW_SIGNAL_PCLK_N|\
		SENSOR_HW_SIGNAL_VSYNC_P|\
		SENSOR_HW_SIGNAL_HSYNC_P,			//bit0: 0:negative; 1:positive -> polarily of pixel clock
	// bit2: 0:negative; 1:positive -> polarily of horizontal synchronization signal
	// bit4: 0:negative; 1:positive -> polarily of vertical synchronization signal
	// other bit: reseved

	// preview mode
	SENSOR_ENVIROMENT_NORMAL|\
		SENSOR_ENVIROMENT_NIGHT|\
		SENSOR_ENVIROMENT_SUNNY,

	// image effect
	SENSOR_IMAGE_EFFECT_NORMAL|\
		SENSOR_IMAGE_EFFECT_BLACKWHITE|\
		SENSOR_IMAGE_EFFECT_RED|\
		SENSOR_IMAGE_EFFECT_GREEN|\
		SENSOR_IMAGE_EFFECT_BLUE|\
		SENSOR_IMAGE_EFFECT_YELLOW|\
		SENSOR_IMAGE_EFFECT_NEGATIVE|\
		SENSOR_IMAGE_EFFECT_CANVAS,

	// while balance mode
	0,

	7,								// bit[0:7]: count of step in brightness, contrast, sharpness, saturation
	// bit[8:31] reseved

	SENSOR_LOW_PULSE_RESET,		// reset pulse level
	100,						//reset pulse width(ms)

	SENSOR_HIGH_LEVEL_PWDN,			// 1: high level valid; 0: low level valid
	2,
	{{0x02, 0x2b},
		{0x02, 0x2b}},

	SENSOR_AVDD_2800MV,				// voltage of avdd

	640,							// max width of source image
	480,							// max height of source image
	"SP0A20",						// name of sensor

	SENSOR_IMAGE_FORMAT_YUV422,		// define in SENSOR_IMAGE_FORMAT_E enum,
	// if set to SENSOR_IMAGE_FORMAT_MAX here, image format depent on SENSOR_REG_TAB_INFO_T
	SENSOR_IMAGE_PATTERN_YUV422_YUYV,	// pattern of input image form sensor;

	s_sp0a20_resolution_Tab_YUV,	// point to resolution table information structure
	&s_sp0a20_ioctl_func_tab,		// point to ioctl function table

	PNULL,							// information and table about Rawrgb sensor
	PNULL,							// extend information about sensor
	SENSOR_AVDD_1800MV,				//iovdd
	SENSOR_AVDD_1800MV,                      // dvdd   //SENSOR_AVDD_1800MV
	1,						//skip frame num before preview//2
	2,						//skip frame num before capture2//2
	0,			//skip frame num during preview
	0,			//skip frame num during video preview
	0,                     // threshold enable(only analog TV)
	0,                     // atv output mode 0 fix mode 1 auto mode
	0,                    // atv output start postion
	0,                     // atv output end postion
	0,
	{SENSOR_INTERFACE_TYPE_CSI2, 1, 8, 1},
	PNULL,
	3,			// skip frame num while change setting
};
/**---------------------------------------------------------------------------*
 ** 							Function  Definitions
 **---------------------------------------------------------------------------*/
LOCAL void sp0a20_WriteReg( uint8_t  subaddr, uint8_t data )
{
#ifndef	_USE_DSP_I2C_
	Sensor_WriteReg_8bits(subaddr, data);
#else
	DSENSOR_IICWrite((uint16_t)subaddr, (uint16_t)data);
#endif

	SENSOR_TRACE("SENSOR: sp0a20_WriteReg reg/value(%x,%x) !!\n", subaddr, data);
}
LOCAL uint8_t sp0a20_ReadReg( uint8_t  subaddr)
{
	uint8_t value = 0;

#ifndef	_USE_DSP_I2C_
	value = Sensor_ReadReg( subaddr);
#else
	value = (uint16_t)DSENSOR_IICRead((uint16_t)subaddr);
#endif

	SENSOR_TRACE("SENSOR: sp0a20_ReadReg reg/value(%x,%x) !!\n", subaddr, value);
	return value;
}
LOCAL void SP0A20_Write_Group_Regs( SENSOR_REG_T* sensor_reg_ptr )
{
    uint32_t i;
    
    for(i = 0; (0xFF != sensor_reg_ptr[i].reg_addr) || (0xFF != sensor_reg_ptr[i].reg_value) ; i++)
    {
        Sensor_WriteReg(sensor_reg_ptr[i].reg_addr, sensor_reg_ptr[i].reg_value);
    }

}
static uint32_t SP0A20_PowerOn(uint32_t power_on)
{
	SENSOR_AVDD_VAL_E dvdd_val = g_SP0A20_yuv_info.dvdd_val;
	SENSOR_AVDD_VAL_E avdd_val = g_SP0A20_yuv_info.avdd_val;
	SENSOR_AVDD_VAL_E iovdd_val = g_SP0A20_yuv_info.iovdd_val;
	BOOLEAN power_down = g_SP0A20_yuv_info.power_down_level;
	BOOLEAN reset_level = g_SP0A20_yuv_info.reset_pulse_level;

	CMR_LOGV("%s power_on = %d, power_down_level = %d, avdd_val = %d\n",
			__func__, power_on, power_down, avdd_val);

	if (SENSOR_TRUE == power_on) {
		Sensor_PowerDown(power_down);
		// Open power
		Sensor_SetMonitorVoltage(SENSOR_AVDD_2800MV);
		Sensor_SetVoltage(dvdd_val, avdd_val, iovdd_val);
		usleep(20*1000);
		//_dw9174_SRCInit(2);
		Sensor_SetMCLK(SENSOR_DEFALUT_MCLK);
		usleep(10*1000);
		Sensor_PowerDown(!power_down);
		usleep(10*1000);
		// Reset sensor
		Sensor_Reset(reset_level);
		usleep(20*1000);
	} else {
		Sensor_PowerDown(power_down);
		Sensor_SetMCLK(SENSOR_DISABLE_MCLK);
		Sensor_SetVoltage(SENSOR_AVDD_CLOSED, SENSOR_AVDD_CLOSED, SENSOR_AVDD_CLOSED);
		Sensor_SetMonitorVoltage(SENSOR_AVDD_CLOSED);
	}
	SENSOR_PRINT("(1:on, 0:off): %d", power_on);
	return (uint32_t)SENSOR_SUCCESS;
}

LOCAL uint32_t sp0a20_Identify(uint32_t param)
{
	uint16_t iden_reg_val = 0;
	uint8_t ret = SENSOR_FAIL;
	sp0a20_WriteReg(0xfd,0x00);
	iden_reg_val = sp0a20_ReadReg(0x02);
	SENSOR_PRINT("%s sensor_id is %x\n", __func__, iden_reg_val);
	if(0x2b == iden_reg_val)
	{
		ret = SENSOR_SUCCESS;

	}
#ifdef SP0A20_TCARD_TEST
	_sp0A20_sdcard();
#endif

	return (uint32_t)ret;
}

LOCAL uint32_t set_sp0a20_ae_enable(uint32_t enable)
{
	#define AE_ENABLE  (0x32)
	unsigned char ae_value;
	sp0a20_WriteReg(0xfd,0x00);
	ae_value=sp0a20_ReadReg(AE_ENABLE);

	if(0x00==enable)
	{
		ae_value &= 0xf8;
		sp0a20_WriteReg(AE_ENABLE,ae_value);
	}
	else if(0x01==enable)
	{
		ae_value|=0x05;
		sp0a20_WriteReg(AE_ENABLE,ae_value);
	}
	return 0;
}

LOCAL uint32_t set_hmirror_enable(uint32_t enable)
{
	return 0;
}

LOCAL uint32_t set_vmirror_enable(uint32_t enable)
{
	return 0;
}

/******************************************************************************/
// Description: set brightness
// Global resource dependence:
// Author:
// Note:
//		level  must smaller than 8
/******************************************************************************/
SENSOR_REG_T sp0a20_brightness_tab[][3]=
{
	{
		{0xfd,0x01},
		{0xdb,0xd0},//level -3
		{0xff,0xff},
	},

	{
		{0xfd,0x01},
		{0xdb,0xe0},//level -2
		{0xff,0xff},
	},

	{
		{0xfd,0x01},
		{0xdb,0xf0},//level -1
		{0xff,0xff},
	},

	{
		{0xfd,0x01},
		{0xdb,0x00},//level 0
		{0xff,0xff},
	},

	{
		{0xfd,0x01},
		{0xdb,0x10},//level +1
		{0xff,0xff},
	},

	{
		{0xfd,0x01},
		{0xdb,0x20},//level +2
		{0xff,0xff},
	},
    {		
		{0xfd,0x01},
		{0xdb,0x30},//level +3
		{0xff,0xff},
	},
};

LOCAL uint32_t set_brightness(uint32_t level)
{
	uint16_t i;
	SENSOR_REG_T* sensor_reg_ptr = (SENSOR_REG_T*)sp0a20_brightness_tab[level];

        if(level>6)
                return 0;

        for(i = 0; (0xFF != sensor_reg_ptr[i].reg_addr) && (0xFF != sensor_reg_ptr[i].reg_value); i++) 
	{
		Sensor_WriteReg(sensor_reg_ptr[i].reg_addr, sensor_reg_ptr[i].reg_value);
	} 
	return 0;
}

SENSOR_REG_T sp0a20_ev_tab[][4]=
{
	    {
		{0xfd,0x01},   
		{0xeb,0x78-0x30},   // level -3
		{0xec,0x78-0x30},   
    {0xff,0xff},        
	},
	    {
		{0xfd,0x01},   
		{0xeb,0x78-0x20},   // level -2
		{0xec,0x78-0x20},   
    {0xff,0xff},        
	},
  {
		{0xfd,0x01},   
		{0xeb,0x78-0x10},   // level -1
		{0xec,0x78-0x10},   
    {0xff,0xff},        
	},

    {
		{0xfd,0x01},   
		{0xeb,0x78},   // level 0
		{0xec,0x78},   
    {0xff,0xff},        
	},

  {
		{0xfd,0x01},   
		{0xeb,0x78+0x10},   // level +1
		{0xec,0x78+0x10},   
    {0xff,0xff},        
	},

  {
		{0xfd,0x01},   
		{0xeb,0x78+0x20},   // level +2
		{0xec,0x78+0x20},   
    {0xff,0xff},        
	},

    {
		{0xfd,0x01},   
		{0xeb,0x78+0x30},   // level +3
		{0xec,0x78+0x30},   
    {0xff,0xff},        
	},

};

LOCAL uint32_t set_sp0a20_ev(uint32_t level)
{
	uint16_t i; 
	SENSOR_REG_T* sensor_reg_ptr = (SENSOR_REG_T*)sp0a20_ev_tab[level];

        if(level>6)
                return 0;

	for(i = 0; (0xFF != sensor_reg_ptr[i].reg_addr) ||(0xFF != sensor_reg_ptr[i].reg_value) ; i++)
	{
		sp0a20_WriteReg(sensor_reg_ptr[i].reg_addr, sensor_reg_ptr[i].reg_value);
	}
	return 0;
}

/******************************************************************************/
// Description: anti 50/60 hz banding flicker
// Global resource dependence:
// Author:
// Note:
//		level  must smaller than 8
/******************************************************************************/
LOCAL uint32_t set_sp0a20_anti_flicker(uint32_t param )
{
	switch (param) {
	case FLICKER_50HZ:
		Antiflicker = FLICKER_50HZ;
		//capture preview daylight 24M 50hz 15-7FPS    
			sp0a20_WriteReg(0xfd,0x00);
			sp0a20_WriteReg(0x03,0x01);
			sp0a20_WriteReg(0x04,0xc2);
			sp0a20_WriteReg(0x05,0x00);
			sp0a20_WriteReg(0x06,0x00);
			sp0a20_WriteReg(0x07,0x00);
			sp0a20_WriteReg(0x08,0x00);
			sp0a20_WriteReg(0x09,0x02);
			sp0a20_WriteReg(0x0a,0xf4);
			sp0a20_WriteReg(0xfd,0x01);
			sp0a20_WriteReg(0xf0,0x00);
			sp0a20_WriteReg(0xf7,0x4b);
			sp0a20_WriteReg(0x02,0x0e);
			sp0a20_WriteReg(0x03,0x01);
			sp0a20_WriteReg(0x06,0x4b);
			sp0a20_WriteReg(0x07,0x00);
			sp0a20_WriteReg(0x08,0x01);
			sp0a20_WriteReg(0x09,0x00);
			sp0a20_WriteReg(0xfd,0x02);
			sp0a20_WriteReg(0xbe,0x1a);
			sp0a20_WriteReg(0xbf,0x04);
			sp0a20_WriteReg(0xd0,0x1a);
			sp0a20_WriteReg(0xd1,0x04);
		break;
	case FLICKER_60HZ:
		Antiflicker = FLICKER_60HZ;
			//capture preview daylight 24M 60hz 15-7FPS  
			sp0a20_WriteReg(0xfd,0x00);
			sp0a20_WriteReg(0x03,0x01);
			sp0a20_WriteReg(0x04,0x7a);
			sp0a20_WriteReg(0x05,0x00);
			sp0a20_WriteReg(0x06,0x00);
			sp0a20_WriteReg(0x07,0x00);
			sp0a20_WriteReg(0x08,0x00);
			sp0a20_WriteReg(0x09,0x02);
			sp0a20_WriteReg(0x0a,0xe7);
			sp0a20_WriteReg(0xfd,0x01);
			sp0a20_WriteReg(0xf0,0x00);
			sp0a20_WriteReg(0xf7,0x3f);
			sp0a20_WriteReg(0x02,0x11);
			sp0a20_WriteReg(0x03,0x01);
			sp0a20_WriteReg(0x06,0x3f);
			sp0a20_WriteReg(0x07,0x00);
			sp0a20_WriteReg(0x08,0x01);
			sp0a20_WriteReg(0x09,0x00);
			sp0a20_WriteReg(0xfd,0x02);
			sp0a20_WriteReg(0xbe,0x2f);
			sp0a20_WriteReg(0xbf,0x04);
			sp0a20_WriteReg(0xd0,0x2f);
			sp0a20_WriteReg(0xd1,0x04);
		break;
	default:
		break;
	}
	return 0;
}

/******************************************************************************/
// Description: set video mode
// Global resource dependence:
// Author:
// Note:
//
/******************************************************************************/
LOCAL SENSOR_REG_T SP0A20_video_mode_nor_tab_50hz[][25]=
{
	//Video preview
	{
		//capture preview daylight 24M 50hz 15-7FPS   
		{0xfd,0x00},                  //   1
		{0x03,0x01},
		{0x04,0xc2},
		{0x05,0x00},
		{0x06,0x00},        //  5
		{0x07,0x00},
		{0x08,0x00},
		{0x09,0x02},
		{0x0a,0xf4},
		{0xfd,0x01},       //10
		{0xf0,0x00},
		{0xf7,0x4b},
		{0x02,0x0e},
		{0x03,0x01},
		{0x06,0x4b},       //15
		{0x07,0x00},
		{0x08,0x01},
		{0x09,0x00},
		{0xfd,0x02},
		{0xbe,0x1a},      //20
		{0xbf,0x04},
		{0xd0,0x1a},
		{0xd1,0x04},
		{0xfd,0x00},                   
		{0xFF,0xFF},	//那y℅谷?芍那?㊣那??
	},    
	//Video record
	{
		//Video record daylight 24M 50hz 14-14FPS maxgain:0x80
		{0xfd,0x00},
		{0x03,0x01},
		{0x04,0x68},
		{0x05,0x00},
		{0x06,0x00},
		{0x07,0x00},
		{0x08,0x00},
		{0x09,0x04},
		{0x0a,0x84},
		{0xfd,0x01},
		{0xf0,0x00},
		{0xf7,0x3c},
		{0x02,0x08},
		{0x03,0x01},
		{0x06,0x3c},
		{0x07,0x00},
		{0x08,0x01},
		{0x09,0x00},
		{0xfd,0x02},
		{0xbe,0xe0},
		{0xbf,0x01},
		{0xd0,0xe0},
		{0xd1,0x01},
		{0xFF,0xFF},	//那y℅谷?芍那?㊣那??
	},
	// UPCC  mode
	{
		//Video record daylight 24M 50hz 14-14FPS maxgain:0x80
		{0xfd,0x00},
		{0x03,0x01},
		{0x04,0x68},
		{0x05,0x00},
		{0x06,0x00},
		{0x07,0x00},
		{0x08,0x00},
		{0x09,0x04},
		{0x0a,0x84},
		{0xfd,0x01},
		{0xf0,0x00},
		{0xf7,0x3c},
		{0x02,0x08},
		{0x03,0x01},
		{0x06,0x3c},
		{0x07,0x00},
		{0x08,0x01},
		{0x09,0x00},
		{0xfd,0x02},
		{0xbe,0xe0},
		{0xbf,0x01},
		{0xd0,0xe0},
		{0xd1,0x01},
		{0xFF,0xFF},	//那y℅谷?芍那?㊣那??
	},  
};    

LOCAL SENSOR_REG_T SP0A20_video_mode_nor_tab_60hz[][24]=
{
	//Video preview
	{
		//capture preview daylight 24M 60Hz 15-7FPS  
		{0xfd,0x00},  //  1
		{0x03,0x01},
		{0x04,0x7a},
		{0x05,0x00},
		{0x06,0x00},   // 5
		{0x07,0x00},
		{0x08,0x00},
		{0x09,0x02},
		{0x0a,0xe7},
		{0xfd,0x01},   // 10
		{0xf0,0x00},
		{0xf7,0x3f},
		{0x02,0x11},
		{0x03,0x01},
		{0x06,0x3f},  // 15
		{0x07,0x00},
		{0x08,0x01},
		{0x09,0x00},
		{0xfd,0x02},
		{0xbe,0x2f},  // 20 
		{0xbf,0x04},
		{0xd0,0x2f},
		{0xd1,0x04},
		{0xFF,0xFF},	//那y℅谷?芍那?㊣那??
	},    
	//Video record
	{
		//Video record daylight 24M 60Hz 12-12FPS 
		{0xfd,0x00}, // 1
		{0x03,0x01},
		{0x04,0x2c},
		{0x05,0x00},
		{0x06,0x00},  // 5
		{0x07,0x00},
		{0x08,0x00},
		{0x09,0x04},
		{0x0a,0x84},
		{0xfd,0x01},// 10
		{0xf0,0x00},
		{0xf7,0x32},
		{0x02,0x0a},
		{0x03,0x01},
		{0x06,0x32},// 15
		{0x07,0x00},
		{0x08,0x01},
		{0x09,0x00},
		{0xfd,0x02},
		{0xbe,0xf4},// 20
		{0xbf,0x01},
		{0xd0,0xf4},
		{0xd1,0x01},                      
		{0xFF,0xFF},	//那y℅谷?芍那?㊣那??
	},
	// UPCC  mode
	{	
		//Video record daylight 24M 60Hz 12-12FPS 
		{0xfd,0x00},
		{0x03,0x01},
		{0x04,0x2c},
		{0x05,0x00},
		{0x06,0x00},
		{0x07,0x00},
		{0x08,0x00},
		{0x09,0x04},
		{0x0a,0x84},
		{0xfd,0x01},
		{0xf0,0x00},
		{0xf7,0x32},
		{0x02,0x0a},
		{0x03,0x01},
		{0x06,0x32},
		{0x07,0x00},
		{0x08,0x01},
		{0x09,0x00},
		{0xfd,0x02},
		{0xbe,0xf4},
		{0xbf,0x01},
		{0xd0,0xf4},
		{0xd1,0x01},                      
		{0xFF,0xFF},	//“oy?芍“|?“⊿“o??角“o??
	},  
};
LOCAL uint32_t set_sp0a20_video_mode(uint32_t mode)
{
	uint8_t data=0x00;
	uint16_t i;
	SENSOR_REG_T* sensor_reg_ptr = PNULL;
	uint8_t tempregval = 0;
	//SENSOR_PRINTK(" xg:set_SP0A20_video_mode ,%d,%d\r\n",mode,Antiflicker);
	// SENSOR_ASSERT(mode <=DCAMERA_MODE_MAX);

	if(Antiflicker ==FLICKER_50HZ )
	{
		//  if(CHIP_DetectMemType())

		//        sensor_reg_ptr = (SENSOR_REG_T*)SP0A20_video_mode_nor_tab_50hz[mode];

		//  else

		sensor_reg_ptr = (SENSOR_REG_T*)SP0A20_video_mode_nor_tab_50hz[mode];
	}
	else
	{
		// if(CHIP_DetectMemType())

		//        sensor_reg_ptr = (SENSOR_REG_T*)SP0A20_video_mode_nor_tab_60hz[mode];

		//   else

		sensor_reg_ptr = (SENSOR_REG_T*)SP0A20_video_mode_nor_tab_60hz[mode];
	}

	//SENSOR_ASSERT(PNULL != sensor_reg_ptr);

	for(i = 0; (0xFF != sensor_reg_ptr[i].reg_addr) || (0xFF != sensor_reg_ptr[i].reg_value); i++)
	{
		sp0a20_WriteReg(sensor_reg_ptr[i].reg_addr, sensor_reg_ptr[i].reg_value);
	}
	return 0;
}
/******************************************************************************/
// Description: set wb mode
// Global resource dependence:
// Author:
// Note:
//
/******************************************************************************/
SENSOR_REG_T sp0a20_awb_tab[][7]=
{
	//AUTO
	{
		/* Auto: */
		{0xfd, 0x02},
		{0x26, 0xa0},//a0
		{0x27, 0xa8},//a8
		{0xfd, 0x01},
		{0x32, 0x15},
		{0xfd, 0x00},
		{0xff, 0xff},
	},
	//INCANDESCENCE:
	{
		/* Office: *//*INCANDISCENT*///啞喋腑
		{0xfd, 0x01},
		{0x32, 0x05},
		{0xfd, 0x02},
		{0x26, 0x7b},
		{0x27, 0xd3},
		{0xfd, 0x00},
		{0xff, 0xff},
	},
	//U30 ?
	{
		/* Office: *//*INCANDISCENT*///啞喋腑
		{0xfd, 0x01},
		{0x32, 0x05},
		{0xfd, 0x02},
		{0x26, 0x7b},
		{0x27, 0xd3},
		{0xfd, 0x00},
		{0xff, 0xff},
	},
	//CWF ?
	{
		/* Home: */
		{0xfd, 0x01},
		{0x32, 0x05},
		{0xfd, 0x02},
		{0x26, 0xb4},
		{0x27, 0xc4},
		{0xfd, 0x00},	
		{0xff, 0xff},
	},
	//FLUORESCENT:
	{
		/* Home: */
		{0xfd, 0x01},
		{0x32, 0x05},
		{0xfd, 0x02},
		{0x26, 0xb4},
		{0x27, 0xc4},
		{0xfd, 0x00},	
		{0xff, 0xff},
	},
	//SUN:
	{
		/*DAYLIGHT*/
		{0xfd, 0x01},
		{0x32, 0x05},
		{0xfd, 0x02},
		{0x26, 0xc1},
		{0x27, 0x88},
		{0xfd, 0x00},
		{0xff, 0xff},
	},
	//CLOUD:
	{
		/*CLOUDY*/
		{0xfd, 0x01},
		{0x32, 0x05},
		{0xfd, 0x02},
		{0x26, 0xe2},
		{0x27, 0x82},
		{0xfd, 0x00},
		{0xff, 0xff},
	},
};

LOCAL uint32_t set_sp0a20_awb(uint32_t mode)
{
	#if 0
	uint8_t awb_en_value;
	uint16_t i;
	SENSOR_REG_T* sensor_reg_ptr = (SENSOR_REG_T*)sp0a20_awb_tab[mode];

	awb_en_value = sp0a20_ReadReg(0x22);

	if(mode>6)
		return 0;

	for(i = 0; (0xFF != sensor_reg_ptr[i].reg_addr) || (0xFF != sensor_reg_ptr[i].reg_value); i++) {
		if(0x22 == sensor_reg_ptr[i].reg_addr) {
			if(mode == 0)
				sp0a20_WriteReg(0x22, awb_en_value |0x02 );
			else
				sp0a20_WriteReg(0x22, awb_en_value &0xfd );
		} else {
			sp0a20_WriteReg(sensor_reg_ptr[i].reg_addr, sensor_reg_ptr[i].reg_value);
		}
	}
	SENSOR_TRACE("SENSOR: set_awb_mode: mode = %d\n", mode);

	return 0;
	#endif
	SENSOR_REG_T* sensor_reg_ptr = (SENSOR_REG_T*)sp0a20_awb_tab[mode];

	//SCI_ASSERT(mode < 7);
	//SCI_ASSERT(PNULL != sensor_reg_ptr);

	SP0A20_Write_Group_Regs(sensor_reg_ptr);

	//SENSOR_PRINTK("set_SP0A20_awb_mode: mode = %d", mode);

	SENSOR_Sleep(100); 
	return 0;
}

SENSOR_REG_T sp0a20_contrast_tab[][10]=
{
{
		{0xfd, 0x01},
		{0x10, SP0A20_P1_0x10 - 0x30},	//level -3
		{0x11, SP0A20_P1_0x11 - 0x30},
		{0x12, SP0A20_P1_0x12 - 0x30},
		{0x13, SP0A20_P1_0x13 - 0x30},
		{0x14, SP0A20_P1_0x14 - 0x30},
		{0x15, SP0A20_P1_0x15 - 0x30},
		{0x16, SP0A20_P1_0x16 - 0x30},
		{0x17, SP0A20_P1_0x17 - 0x30},
		{0xff, 0xff}
	},

	{
		{0xfd, 0x01},
		{0x10, SP0A20_P1_0x10 - 0x20},	//level -2
		{0x11, SP0A20_P1_0x11 - 0x20},
		{0x12, SP0A20_P1_0x12 - 0x20},
		{0x13, SP0A20_P1_0x13 - 0x20},
		{0x14, SP0A20_P1_0x14 - 0x20},
		{0x15, SP0A20_P1_0x15 - 0x20},
		{0x16, SP0A20_P1_0x16 - 0x20},
		{0x17, SP0A20_P1_0x17 - 0x20},
		{0xff, 0xff}
	},

	{
		{0xfd, 0x01},
		{0x10, SP0A20_P1_0x10 - 0x10},	//level -1
		{0x11, SP0A20_P1_0x11 - 0x10},
		{0x12, SP0A20_P1_0x12 - 0x10},
		{0x13, SP0A20_P1_0x13 - 0x10},
		{0x14, SP0A20_P1_0x14 - 0x10},
		{0x15, SP0A20_P1_0x15 - 0x10},
		{0x16, SP0A20_P1_0x16 - 0x10},
		{0x17, SP0A20_P1_0x17 - 0x10},
		{0xff, 0xff}
	},

	{
		{0xfd, 0x01},
		{0x10, SP0A20_P1_0x10},	//level 0
		{0x11, SP0A20_P1_0x11},
		{0x12, SP0A20_P1_0x12},
		{0x13, SP0A20_P1_0x13},
		{0x14, SP0A20_P1_0x14},
		{0x15, SP0A20_P1_0x15},
		{0x16, SP0A20_P1_0x16},
		{0x17, SP0A20_P1_0x17},
		{0xff, 0xff}
	},

	{
		{0xfd, 0x01},
		{0x10, SP0A20_P1_0x10 + 0x10},	//level +1
		{0x11, SP0A20_P1_0x11 + 0x10},
		{0x12, SP0A20_P1_0x12 + 0x10},
		{0x13, SP0A20_P1_0x13 + 0x10},
		{0x14, SP0A20_P1_0x14 + 0x10},
		{0x15, SP0A20_P1_0x15 + 0x10},
		{0x16, SP0A20_P1_0x16 + 0x10},
		{0x17, SP0A20_P1_0x17 + 0x10},
		{0xff, 0xff}
	},

	{
		{0xfd, 0x01},
		{0x10, SP0A20_P1_0x10 + 0x20},	//level +2
		{0x11, SP0A20_P1_0x11 + 0x20},
		{0x12, SP0A20_P1_0x12 + 0x20},
		{0x13, SP0A20_P1_0x13 + 0x20},
		{0x14, SP0A20_P1_0x14 + 0x20},
		{0x15, SP0A20_P1_0x15 + 0x20},
		{0x16, SP0A20_P1_0x16 + 0x20},
		{0x17, SP0A20_P1_0x17 + 0x20},
		{0xff, 0xff}
	},

	{
		{0xfd, 0x01},
		{0x10, SP0A20_P1_0x10 + 0x30},	//level +3
		{0x11, SP0A20_P1_0x11 + 0x30},
		{0x12, SP0A20_P1_0x12 + 0x30},
		{0x13, SP0A20_P1_0x13 + 0x30},
		{0x14, SP0A20_P1_0x14 + 0x30},
		{0x15, SP0A20_P1_0x15 + 0x30},
		{0x16, SP0A20_P1_0x16 + 0x30},
		{0x17, SP0A20_P1_0x17 + 0x30},
		{0xff, 0xff}
	}, 
};
LOCAL uint32_t set_contrast(uint32_t level)
{
	uint16_t i;    

	SENSOR_REG_T* sensor_reg_ptr = (SENSOR_REG_T*)sp0a20_contrast_tab[level];

        if(level>6)
                return 0;
		
        for(i = 0; (0xFF != sensor_reg_ptr[i].reg_addr) && (0xFF != sensor_reg_ptr[i].reg_value); i++)
	{
		Sensor_WriteReg(sensor_reg_ptr[i].reg_addr, sensor_reg_ptr[i].reg_value);
	}
	return 0;
}
LOCAL uint32_t set_sharpness(uint32_t level)
{
	return 0;
}


SENSOR_REG_T sp0a20_saturation_tab[][10]=
{
	{
		{0xfd,0x01},
		{0xd3,0x80-0x30},
		{0xd4,0x80-0x30},
		{0xd5,0x70-0x30},
		{0xd6,0x60-0x30},
		{0xd7,0x80-0x30},
		{0xd8,0x80-0x30},
		{0xd9,0x70-0x30},
		{0xda,0x60-0x30},
		{0xff,0xff},
	},//level -3

	{
		{0xfd,0x01},
		{0xd3,0x80-0x20},
		{0xd4,0x80-0x20},
		{0xd5,0x70-0x20},
		{0xd6,0x60-0x20},
		{0xd7,0x80-0x20},
		{0xd8,0x80-0x20},
		{0xd9,0x70-0x20},
		{0xda,0x60-0x20},
		{0xff,0xff},
	},//level -2

	{
		{0xfd,0x01},
		{0xd3,0x80-0x10},
		{0xd4,0x80-0x10},
		{0xd5,0x70-0x10},
		{0xd6,0x60-0x10},
		{0xd7,0x80-0x10},
		{0xd8,0x80-0x10},
		{0xd9,0x70-0x10},
		{0xda,0x60-0x10},
		{0xff,0xff},
	},//level -1

	{
		{0xfd,0x01},
		{0xd3,0x80},
		{0xd4,0x80},
		{0xd5,0x70},
		{0xd6,0x60},
		{0xd7,0x80},
		{0xd8,0x80},
		{0xd9,0x70},
		{0xda,0x60},
		{0xff,0xff},
	},//level +0

	{
		{0xfd,0x01},
		{0xd3,0x80+0x10},
		{0xd4,0x80+0x10},
		{0xd5,0x70+0x10},
		{0xd6,0x60+0x10},
		{0xd7,0x80+0x10},
		{0xd8,0x80+0x10},
		{0xd9,0x70+0x10},
		{0xda,0x60+0x10},
		{0xff,0xff+0x10},
	},//level +1

	{
		{0xfd,0x01},
		{0xd3,0x80+0x20},
		{0xd4,0x80+0x20},
		{0xd5,0x70+0x20},
		{0xd6,0x60+0x20},
		{0xd7,0x80+0x20},
		{0xd8,0x80+0x20},
		{0xd9,0x70+0x20},
		{0xda,0x60+0x20},
		{0xff,0xff},
	},//level +2
    {		
		{0xfd,0x01},
		{0xd3,0x80+0x30},
		{0xd4,0x80+0x30},
		{0xd5,0x70+0x30},
		{0xd6,0x60+0x30},
		{0xd7,0x80+0x30},
		{0xd8,0x80+0x30},
		{0xd9,0x70+0x30},
		{0xda,0x60+0x30},
		{0xff,0xff},
	},//level +3
};


LOCAL uint32_t set_saturation(uint32_t level)
{
	uint16_t i;
	SENSOR_REG_T* sensor_reg_ptr = (SENSOR_REG_T*)sp0a20_saturation_tab[level];

        if(level>6)
                return 0;

        for(i = 0; (0xFF != sensor_reg_ptr[i].reg_addr) && (0xFF != sensor_reg_ptr[i].reg_value); i++) 
	{
		sp0a20_WriteReg(sensor_reg_ptr[i].reg_addr, sensor_reg_ptr[i].reg_value);
	} 
	return 0;
}

/******************************************************************************/
// Description: set brightness
// Global resource dependence:
// Author:
// Note:
//		level  must smaller than 8
/******************************************************************************/
LOCAL uint32_t set_preview_mode(uint32_t preview_mode)
{
	switch (preview_mode)
	{
		case DCAMERA_ENVIRONMENT_NORMAL: 
		case DCAMERA_ENVIRONMENT_SUNNY:
			sp0a20_WriteReg(0xfd,0x01 );
			sp0a20_WriteReg(0xcd,0x10);
			sp0a20_WriteReg(0xce,0x1f);
			if(Antiflicker== FLICKER_50HZ)
			{
				
				//SENSOR_PRINTK("normal mode 50hz\r\n");
			//capture preview daylight 24M 50hz 15-7FPS    
			sp0a20_WriteReg(0xfd,0x00);
			sp0a20_WriteReg(0x03,0x01);
			sp0a20_WriteReg(0x04,0xc2);
			sp0a20_WriteReg(0x05,0x00);
			sp0a20_WriteReg(0x06,0x00);
			sp0a20_WriteReg(0x07,0x00);
			sp0a20_WriteReg(0x08,0x00);
			sp0a20_WriteReg(0x09,0x02);
			sp0a20_WriteReg(0x0a,0xf4);
			sp0a20_WriteReg(0xfd,0x01);
			sp0a20_WriteReg(0xf0,0x00);
			sp0a20_WriteReg(0xf7,0x4b);
			sp0a20_WriteReg(0x02,0x0e);
			sp0a20_WriteReg(0x03,0x01);
			sp0a20_WriteReg(0x06,0x4b);
			sp0a20_WriteReg(0x07,0x00);
			sp0a20_WriteReg(0x08,0x01);
			sp0a20_WriteReg(0x09,0x00);
			sp0a20_WriteReg(0xfd,0x02);
			sp0a20_WriteReg(0xbe,0x1a);
			sp0a20_WriteReg(0xbf,0x04);
			sp0a20_WriteReg(0xd0,0x1a);
			sp0a20_WriteReg(0xd1,0x04);			
			}
			else	
			{
				//SENSOR_PRINTK("normal mode 60hz\r\n");
				//capture preview daylight 24M 60hz 15-7FPS  
			sp0a20_WriteReg(0xfd,0x00);
			sp0a20_WriteReg(0x03,0x01);
			sp0a20_WriteReg(0x04,0x7a);
			sp0a20_WriteReg(0x05,0x00);
			sp0a20_WriteReg(0x06,0x00);
			sp0a20_WriteReg(0x07,0x00);
			sp0a20_WriteReg(0x08,0x00);
			sp0a20_WriteReg(0x09,0x02);
			sp0a20_WriteReg(0x0a,0xe7);
			sp0a20_WriteReg(0xfd,0x01);
			sp0a20_WriteReg(0xf0,0x00);
			sp0a20_WriteReg(0xf7,0x3f);
			sp0a20_WriteReg(0x02,0x11);
			sp0a20_WriteReg(0x03,0x01);
			sp0a20_WriteReg(0x06,0x3f);
			sp0a20_WriteReg(0x07,0x00);
			sp0a20_WriteReg(0x08,0x01);
			sp0a20_WriteReg(0x09,0x00);
			sp0a20_WriteReg(0xfd,0x02);
			sp0a20_WriteReg(0xbe,0x2f);
			sp0a20_WriteReg(0xbf,0x04);
			sp0a20_WriteReg(0xd0,0x2f);
			sp0a20_WriteReg(0xd1,0x04);
			}	
			break;

		case DCAMERA_ENVIRONMENT_NIGHT:
			sp0a20_WriteReg(0xfd,0x01 );
			sp0a20_WriteReg(0xcd,0x25);
			sp0a20_WriteReg(0xce,0x1f);
			if(Antiflicker== FLICKER_50HZ)
			{
				//SENSOR_PRINTK("night mode 50hz\r\n");
				//capture preview night 24M 50hz 12-6FPS 
			sp0a20_WriteReg(0xfd,0x00);
			sp0a20_WriteReg(0x03,0x01);
			sp0a20_WriteReg(0x04,0x68);
			sp0a20_WriteReg(0x05,0x00);
			sp0a20_WriteReg(0x06,0x00);
			sp0a20_WriteReg(0x07,0x00);
			sp0a20_WriteReg(0x08,0x00);
			sp0a20_WriteReg(0x09,0x04);
			sp0a20_WriteReg(0x0a,0x84);
			sp0a20_WriteReg(0xfd,0x01);
			sp0a20_WriteReg(0xf0,0x00);
			sp0a20_WriteReg(0xf7,0x3c);
			sp0a20_WriteReg(0x02,0x10);
			sp0a20_WriteReg(0x03,0x01);
			sp0a20_WriteReg(0x06,0x3c);
			sp0a20_WriteReg(0x07,0x00);
			sp0a20_WriteReg(0x08,0x01);
			sp0a20_WriteReg(0x09,0x00);
			sp0a20_WriteReg(0xfd,0x02);
			sp0a20_WriteReg(0xbe,0xc0);
			sp0a20_WriteReg(0xbf,0x03);
			sp0a20_WriteReg(0xd0,0xc0);
			sp0a20_WriteReg(0xd1,0x03);
			}
			else
			{
				//SENSOR_PRINTK("night mode 60hz\r\n");
				//capture preview night 24M 60hz 12-6FPS 
			sp0a20_WriteReg(0xfd,0x00);
			sp0a20_WriteReg(0x03,0x01);
			sp0a20_WriteReg(0x04,0x2c);
			sp0a20_WriteReg(0x05,0x00);
			sp0a20_WriteReg(0x06,0x00);
			sp0a20_WriteReg(0x07,0x00);
			sp0a20_WriteReg(0x08,0x00);
			sp0a20_WriteReg(0x09,0x04);
			sp0a20_WriteReg(0x0a,0x84);
			sp0a20_WriteReg(0xfd,0x01);
			sp0a20_WriteReg(0xf0,0x00);
			sp0a20_WriteReg(0xf7,0x32);
			sp0a20_WriteReg(0x02,0x14);
			sp0a20_WriteReg(0x03,0x01);
			sp0a20_WriteReg(0x06,0x32);
			sp0a20_WriteReg(0x07,0x00);
			sp0a20_WriteReg(0x08,0x01);
			sp0a20_WriteReg(0x09,0x00);
			sp0a20_WriteReg(0xfd,0x02);
			sp0a20_WriteReg(0xbe,0xe8);
			sp0a20_WriteReg(0xbf,0x03);
			sp0a20_WriteReg(0xd0,0xe8);
			sp0a20_WriteReg(0xd1,0x03);
			}
			break;

		default:
			break;
}
		usleep(100);
		return 0;
}

SENSOR_REG_T sp0a20_image_effect_tab[][8]=
{
	  // effect normal
    {
		{0xfd,0x01},  // 1
		{0x66,0x00},
		{0x67,0x80},
		{0x68,0x80},
		{0xdf,0x00},
		{0xfd,0x02},
		{0x14,0x00},
		{0xff, 0xff} // 8
    },
    //effect BLACKWHITE//GRAYSCALE
    {
		{0xfd,0x01},
		{0x66,0x20},
		{0x67,0x80},
		{0x68,0x80},
		{0xdf,0x00},
		{0xfd,0x02},
		{0x14,0x00},
		{0xff, 0xff}
    },
    // effect RED pink
    {
    		{0xfd,0x01},
		{0x66,0x10},
		{0x67,0x58},
		{0x68,0x98},
		{0xdf,0x00},
		{0xfd,0x02},
		{0x14,0x00},
		{0xff, 0xff}
    },
    // effect GREEN//SEPIAGREEN
    {
		{0xfd,0x01},
		{0x66,0x10},
		{0x67,0x50},
		{0x68,0x50},
		{0xdf,0x00},
		{0xfd,0x02},
		{0x14,0x00},
		{0xff, 0xff}			

    },
    // effect  BLUE//SEPIABLUE
    {
		{0xfd,0x01},
		{0x66,0x10},
		{0x67,0x80},
		{0x68,0xb0},
		{0xdf,0x00},
		{0xfd,0x02},
		{0x14,0x00},
		{0xff, 0xff}	
    },
    // effect  YELLOW//SEPIAYELLOW
    {
		{0xfd,0x01},
		{0x66,0x10},
		{0x67,0x98},
		{0x68,0x58},
		{0xdf,0x00},
		{0xfd,0x02},
		{0x14,0x00},
		{0xff, 0xff}	
    },  
    // effect NEGATIVE//COLORINV
    {
		{0xfd,0x01},
		{0x66,0x04},
		{0x67,0x80},
		{0x68,0x80},
		{0xdf,0x00},
		{0xfd,0x02},
		{0x14,0x00},
		{0xff, 0xff}		

    },    
    //effect ANTIQUE//SEPIA
    {
		{0xfd,0x01},
		{0x66,0x10},
		{0x67,0x98},
		{0x68,0x58},
		{0xdf,0x00},
		{0xfd,0x02},
		{0x14,0x00},
		{0xff, 0xff}		
    },
};

LOCAL uint32_t set_image_effect(uint32_t effect_type)
{
	uint16_t i;

	SENSOR_REG_T* sensor_reg_ptr = (SENSOR_REG_T*)sp0a20_image_effect_tab[effect_type];
        if(effect_type>7)
                return 0;

	for(i = 0; (0xFF != sensor_reg_ptr[i].reg_addr) || (0xFF != sensor_reg_ptr[i].reg_value) ; i++)
	{
		Sensor_WriteReg(sensor_reg_ptr[i].reg_addr, sensor_reg_ptr[i].reg_value);
	}
	return 0;
}

LOCAL uint32_t sp0a20_StreamOn(uint32_t param)
{
	CMR_LOGE("come in\n");

	Sensor_WriteReg(0xfd, 0x00);
	Sensor_WriteReg(0x92, 0x01);//01
	Sensor_WriteReg(0xfd, 0x01);
	Sensor_WriteReg(0x36, 0x00);
	Sensor_WriteReg(0xfd, 0x00);

	usleep(10*1000);

	return 0;
}

LOCAL uint32_t sp0a20_StreamOff(uint32_t param)
{
	CMR_LOGE("come in\n");

	Sensor_WriteReg(0xfd, 0x00);
	Sensor_WriteReg(0x92, 0x70);
	Sensor_WriteReg(0xfd, 0x01);
	Sensor_WriteReg(0x36, 0x02);
	Sensor_WriteReg(0xfd, 0x00);
	Sensor_WriteReg(0xe7, 0x03);
	Sensor_WriteReg(0xe7, 0x00);
	Sensor_WriteReg(0xfd, 0x00);
	usleep(10*1000);

	return 0;
}

LOCAL uint32_t read_ev_value(uint32_t value)
{
	return 0;
}

LOCAL uint32_t write_ev_value(uint32_t exposure_value)
{
	return 0;
}

LOCAL uint32_t read_gain_value(uint32_t value)
{
	return 0;
}

LOCAL uint32_t write_gain_value(uint32_t gain_value)
{
	return 0;
}

LOCAL uint32_t read_gain_scale(uint32_t value)
{
	return SENSOR_GAIN_SCALE;
}

LOCAL uint32_t set_frame_rate(uint32_t param)
{
	//sp0a20_WriteReg( 0xd8, uint8_t data );
	return 0;
}
#if 0
struct sensor_drv_cfg sensor_sp0a20 = {
	.sensor_pos = CONFIG_DCAM_SENSOR_POS_sp0a20,
	.sensor_name = "sp0a20",
	.driver_info = &g_SP0A20_yuv_info,
};

static int __init sensor_sp0a20_init(void)
{
	return dcam_register_sensor_drv(&sensor_sp0a20);
}

subsys_initcall(sensor_sp0a20_init);
#endif
LOCAL SENSOR_IOCTL_FUNC_TAB_T s_sp0a20_ioctl_func_tab = {
#ifdef SP0A20_TCARD_TEST

	.power = SP0A20_PowerOn,
	.identify = sp0a20_Identify,
	.set_video_mode = set_sp0a20_video_mode,
	.get_trim = sp0a20_GetResolutionTrimTab,
	.stream_on = sp0a20_StreamOn,
	.stream_off = sp0a20_StreamOff,

#else

	.power = SP0A20_PowerOn,
	.identify = sp0a20_Identify,
	.ae_enable =set_sp0a20_ae_enable,
	.hmirror_enable =set_hmirror_enable,
	.vmirror_enable = set_vmirror_enable,
	.set_brightness =set_brightness,
	.set_contrast =set_contrast,
	.set_saturation=set_saturation,
	.set_preview_mode =set_preview_mode,
	.set_image_effect =set_image_effect,
	.set_wb_mode =set_sp0a20_awb,
	.set_exposure_compensation =set_sp0a20_ev,
	.set_anti_banding_flicker =set_sp0a20_anti_flicker,
	.set_video_mode = set_sp0a20_video_mode,
	.get_trim = sp0a20_GetResolutionTrimTab,
	.stream_on = sp0a20_StreamOn,
	.stream_off = sp0a20_StreamOff,
	
#endif
};

