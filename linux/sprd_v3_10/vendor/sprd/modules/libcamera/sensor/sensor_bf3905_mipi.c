/******************************************************************************
 ** Copyright (c) 
 ** File Name:        sensor_BF3905.c                                           *
 ** Author:                                                       *
 ** DATE:                                                              *
 ** Description:   This file contains driver for sensor BF3905. 
 **                                                          
 ******************************************************************************

 ******************************************************************************
 **                        Edit History                                       *
 ** ------------------------------------------------------------------------- *
 ** DATE	2014.10.16	   NAME  BYD  ren yong xing DESCRIPTION    FOR: SC7731                            *
 **       
 ******************************************************************************/

/**---------------------------------------------------------------------------*
 **                         Dependencies                                      *
 **---------------------------------------------------------------------------*/
#include "sensor.h"
#include "jpeg_exif_header.h"
#include "sensor_drv_u.h"
 
/*lint -save -e765 */
/**--------------------------------------------------------------------------*
 **                         Compiler Flag                                      *
 **---------------------------------------------------------------------------*/
#ifdef     __cplusplus
extern     "C" 
{
#endif
 
/**---------------------------------------------------------------------------*
 **                         Const variables                                   *
 **---------------------------------------------------------------------------*/

/**---------------------------------------------------------------------------*
 **                            Macro Define
 **---------------------------------------------------------------------------*/
#define BF3905_I2C_ADDR_W        0x6e//(0xdc/2)
#define BF3905_I2C_ADDR_R         0x6e//(0xdc/2)


static uint32_t  g_flash_mode_en = 0;

/**---------------------------------------------------------------------------*
 **                     Local Function Prototypes                              *
 **---------------------------------------------------------------------------*/

LOCAL uint32_t _BF3905_PowerOn(uint32_t power_on);
LOCAL uint32_t _BF3905_Identify(uint32_t param);
LOCAL uint32_t _BF3905_GetResolutionTrimTab(uint32_t param);
LOCAL uint32_t _BF3905_set_brightness(uint32_t level);
LOCAL uint32_t _BF3905_set_contrast(uint32_t level);
LOCAL uint32_t _BF3905_set_saturation(uint32_t level);
LOCAL uint32_t _BF3905_set_image_effect(uint32_t effect_type);
LOCAL uint32_t _BF3905_set_ev(uint32_t level); 
LOCAL uint32_t _BF3905_set_anti_flicker(uint32_t mode);
LOCAL uint32_t _BF3905_set_video_mode(uint32_t mode);
LOCAL uint32_t _BF3905_set_awb(uint32_t mode);
LOCAL uint32_t _BF3905_set_preview_mode(uint32_t preview_mode);
LOCAL uint32_t _BF3905_BeforeSnapshot(uint32_t sensor_snapshot_mode);
LOCAL uint32_t _BF3905_After_Snapshot(uint32_t param);
LOCAL void BF3905_set_shutter();
LOCAL uint32_t _BF3905_flash(uint32_t param);
LOCAL uint32_t _BF3905_StreamOn(uint32_t param);
LOCAL uint32_t _BF3905_StreamOff(uint32_t param);

LOCAL const SENSOR_REG_T BF3905_YUV_COMMON[]=
{
	{0x12,0x80},
	{SENSOR_WRITE_DELAY, 0x20},//delay 10ms  Add some delay after register reset
	{0x15,0x12},
	{0x3e,0x83},
	{0x12,0x00},
	{0xf3,0x1e},
	{0x3a,0x22},   //20
	{0x1b,0x2e},   //  2e  jin bu le cam
	{0x2a,0x00},
	{0x2b,0x06}, // 0x10

	{0x20,0x09}, ///09   //2014.1.25     ///49
	//{0x09,0x01}, ///00 //6f//60//6c  //2014.1.25   

	{0x17,0x00},
	{0x18,0xa0},
	{0x19,0x00},
	{0x1a,0x78},
	{0x03,0x00},
	{0x5d,0xb3},
	{0xbf,0x08},
	{0xc3,0x08},
	{0xca,0x10},
	{0x62,0x00},
	{0x63,0x00},

	{0xc0,0x0a},// 14   //ryx 2015.04 .03 //pa
	{0xc1,0x50},// 70  //ryx 2015.04 .03
	{0x1e,0x70},////40/ryx   ///70  yzx 1.6

	{0xd9,0x25},
	{0xdf,0x25},
	{0x4a,0x0c},
	{0xda,0x00},
	{0xdb,0xa2},
	{0xdc,0x00},
	{0xdd,0x7a},
	{0xde,0x00},
	{0x60,0xe5},
	{0x61,0xf2},
	{0x6d,0xc0},
	{0xb9,0x00},
	{0x64,0x00},
	{0xbb,0x10},
	{0x08,0x02},
	{0x21,0x4f},
	{0x3e,0x83},
	{0x16,0xa1},
	{0x2f,0xc4},
	//{0x13,0x00}, wzb
	{0x01,0x15},
	{0x02,0x23},
	{0x9d,0x20},
	{0x8c,0x03},
	{0x8d,0x11},
	{0x33,0x10},
	{0x34,0x1d},
	{0x36,0x45},
	{0x6e,0x20},
	{0xbc,0x0d},
	{0x35,0x30},
	{0x65,0x2a},
  	{0x66,0x30},  // 2a   //ryx 2014.10 .24
	{0xbd,0xf4},
	{0xbe,0x44},
	{0x9b,0xf4},
	{0x9c,0x44},
	{0x37,0xf4},
	{0x38,0x44},

	{0x71,0x0f},
	{0x72,0x4c},
	{0x73,0x37},//27 //ryx  2014.08.22
	{0x75,0x8a},
	{0x76,0x98},
	{0x77,0x5a},
	{0x78,0xff},
	{0x79,0x24},///64 //ryx  2014.08.22
	{0x7a,0x22},///12 //ryx  2014.08.22
	{0x7b,0x58},
	{0x7c,0x55},
	{0x7d,0x00},
	{0x7e,0x84},
	{0x7f,0x3c},
	{0x13,0x07},
	{0x24,0x45},///4f //48ryx  2014.08.22
	{0x25,0x88},
	{0x80,0x92},
	{0x81,0x00},
	{0x82,0x2a},
	{0x83,0x54},
	{0x84,0x39},
	{0x85,0x5d},
	{0x86,0x80},
	{0x87,0x14},// 0x16 //ryx 2014.11 .05
	{0x89,0x7d},//63  yzx 1.6//b3//ryx   //   yzx 4.12
	{0x8a,0x4c},//ryx  2014.08.22
	{0x8b,0x3f}, //ryx  2014.08.22
	{0x8f,0x82},
	{0x94,0x82},//42->92 avoid ae vabration //92
	{0x95,0x84},
	{0x96,0xb3},
	{0x97,0x40},
	{0x98,0x9b},// 8a
	{0x99,0x10},
	{0x9a,0x50},
	{0x9f,0x63},     //64   //ryx 2014.11 .05  ///*ISO_400*/  ////	{{0x9f,0x63},},/*ISO_200*/  ////	{{0x9f,0x61},},/*ISO_AUTO*/
	{0x39,0x98},     ///98   //ryx
	{0x3f,0x98},    ///98 //ryx
	{0x90,0x20},
	{0x91,0xd0},

#if 0
	//gamma1   default  
	{0x40,0x3b},
	{0x41,0x36},
	{0x42,0x2b},
	{0x43,0x1d},
	{0x44,0x1a},
	{0x45,0x14},
	{0x46,0x11},
	{0x47,0x0e},
	{0x48,0x0d},
	{0x49,0x0c},
	{0x4b,0x0b},
	{0x4c,0x09},
	{0x4e,0x08},
	{0x4f,0x07},
	{0x50,0x07},
#endif

#if 0                                                                           
	//gamma 1guobao du hao                                         
	{0x40, 0x36},                
	{0x41, 0x33},                 
	{0x42, 0x2a},                 
	{0x43, 0x22},                 
	{0x44, 0x1b},                 
	{0x45, 0x16},                 
	{0x46, 0x13},
	{0x47, 0x10},
	{0x48, 0x0e},
	{0x49, 0x0c},
	{0x4b, 0x0b},
	{0x4c, 0x0a},
	{0x4e, 0x09},
	{0x4f, 0x08},
	{0x50, 0x08},     
#endif 

#if 1                                                                         
	//gamma  clear brighting                                                        
	{0x40, 0x20},
	{0x41, 0x28},
	{0x42, 0x26},
	{0x43, 0x25},
	{0x44, 0x1f},
	{0x45, 0x1a},
	{0x46, 0x16},
	{0x47, 0x12},
	{0x48, 0x0f},
	{0x49, 0x0D},
	{0x4b, 0x0b},
	{0x4c, 0x0a},
	{0x4e, 0x08},
	{0x4f, 0x06},
	{0x50, 0x06},
#endif 

#if 0
	//gamma  low denoise                	
	{0x40, 0x24},	
	{0x41, 0x30},	
	{0x42, 0x24},	
	{0x43, 0x1d},	
	{0x44, 0x1a},	
	{0x45, 0x14},	
	{0x46, 0x11},	
	{0x47, 0x0e},	
	{0x48, 0x0d},	
	{0x49, 0x0c},	
	{0x4b, 0x0b},	
	{0x4c, 0x09},	
	{0x4e, 0x09},	
	{0x4f, 0x08},	
	{0x50, 0x07},

#endif

	//color   out door
	{0x5a,0x56},
	{0x51,0x13},
	{0x52,0x05},
	{0x53,0x91},
	{0x54,0x72},
	{0x57,0x96},
	{0x58,0x35},
	
	//color  fu se hao 
	{0x5a,0xd6},
	{0x51,0x17},
	{0x52,0x13},
	{0x53,0x5e},
	{0x54,0x38},
	{0x57,0x38},
	{0x58,0x02},
	
	/*
	//color  indoor
	{0x5a,0xd6},
	{0x51,0x29},
	{0x52,0x0D},
	{0x53,0x91},
	{0x54,0x81},
	{0x57,0x56},
	{0x58,0x09},*/

	{0x5b,0x02},
	{0x5c,0x30},
	{0xb0,0xe0},
	{0xb3,0x5a}, //58  //ryx
	
	{0xb4,0xe3},
	{0xb1,0xd0},//ryx 2014.11 .05
	{0xb2,0xb0},  //ef  //ryx 2014.11 .05
	
	{0xb4,0x63},
	
	{0xb1,0xa8},//b0 
	{0xb2,0x90},//a0 
	{0x55,0x85},//00 
	{0x56,0x3c},	//middle   //40  yzx  1.6
	{0x6a,0x81},

	{0x70,0x0b},
	{0x69,0x00}, // effect normal 
	{0x67,0x80}, // Normal, 
	{0x68,0x80}, // Normal, 
	{0xb4,0x63}, // Normal, 
	{0x23,0x55},
	{0xa0,0x00},
	{0xa1,0x31},
	{0xa2,0x0a}, //0d
	{0xa3,0x26},// 27
	{0xa4,0x08},//a1 //ryx  2014.08.22
	{0xa5,0x26},///2c //22//0x23
	{0xa6,0x04},
	{0xa7,0x1a},
	{0xa8,0x18},
	{0xa9,0x13},
	{0xaa,0x18},
	{0xab,0x24},
	{0xac,0x3c},
	{0xad,0xf0},
	{0xae,0x59},
	{0xc5,0xaa},
	{0xc6,0xbb},
	{0xc7,0x30},
	{0xc8,0x0f},  //0d
	{0xc9,0x15},  //10
	{0xd0,0xa3},
	{0xd1,0x00},
	{0xd2,0x58},
	{0xd3,0x09},
	{0xd4,0x24},
	{0xee,0x30},
	{0x09,0x01},
	{0xff,0xff},
} ; 


//640*480 YUV   Mode
LOCAL const SENSOR_REG_T BF3905_640x480[] =
{
{0xff, 0xff},
{0xff, 0xff},
	
} ; 



LOCAL SENSOR_REG_TAB_INFO_T s_BF3905_resolution_Tab_YUV[]=
{    
	// COMMON INIT
	{ADDR_AND_LEN_OF_ARRAY(BF3905_YUV_COMMON),   0, 0, 24, SENSOR_IMAGE_FORMAT_YUV422}, 

	// YUV422 PREVIEW 1 
	{ADDR_AND_LEN_OF_ARRAY(BF3905_640x480),   640,  480,   24, SENSOR_IMAGE_FORMAT_YUV422}, 
	{PNULL, 0, 0, 0, 0, 0},
	{PNULL, 0, 0, 0, 0, 0},
	{PNULL, 0, 0, 0, 0, 0},

	// YUV422 PREVIEW 2 
	{PNULL, 0, 0, 0, 0, 0},
	{PNULL, 0, 0, 0, 0, 0},
	{PNULL, 0, 0, 0, 0, 0},
	{PNULL, 0, 0, 0, 0, 0}
};


LOCAL SENSOR_TRIM_T s_BF3905_Resolution_Trim_Tab[]=
{    

	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 640, 480, 130, 60, 510, {0, 0, 640, 480}},//  //ryx add  SIX  //LINE TIME  ( (784+6=790)*1/6000000 )=790/6=130    *10  // pclk*10 =bps  \B1\A3?\B6?\C4 //Frame  LINE  510+0=510
	//{0, 0, 640, 480, 65, 120, 510, {0, 0, 640, 480}},//\B5\DA6\B8\F6\B2\CE\CA\FD\CA\C7bps\A3\AC\B0\D1\D5\E2\B8\F6??\B8\C4\D5\FDȷ//0X1B=0X0E
	////ryx add  SIX  //LINE TIME  ( (784+6=790)*1/12000000 )=790/12=130    *10  // pclk*10 =bps  \B1\A3?\B6?\C4 //Frame  LINE  510+0=510
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}}

};

LOCAL SENSOR_IOCTL_FUNC_TAB_T s_BF3905_ioctl_func_tab = 
{
	// Internal 
	PNULL,
	_BF3905_PowerOn,
	PNULL,
	_BF3905_Identify,

	PNULL,            // write register
	PNULL,            // read  register    
	PNULL,
	_BF3905_GetResolutionTrimTab,

	// External
	PNULL,
	PNULL,
	PNULL,

	_BF3905_set_brightness,//PNULL,//_BF3905_set_brightness,
	_BF3905_set_contrast,//PNULL,//_BF3905_set_contrast,
	PNULL,
	PNULL,//_BF3905_set_saturation,

	_BF3905_set_preview_mode,// PNULL,  
	_BF3905_set_image_effect,// PNULL,//_BF3905_set_image_effect,

	_BF3905_BeforeSnapshot,
	_BF3905_After_Snapshot,
	PNULL,//_BF3905_flash,
	PNULL,
	PNULL,
	PNULL,
	PNULL,
	PNULL,
	PNULL,
	PNULL,
	PNULL,
	_BF3905_set_awb,//PNULL,//_BF3905_set_awb,
	PNULL,
	PNULL,
	_BF3905_set_ev,//PNULL,//_BF3905_set_ev,
	PNULL,// _BF3905_check_image_format_support,
	PNULL,
	PNULL,
	PNULL,//_BF3905_GetExifInfo,
	PNULL,//_BF3905_ExtFunc,
	_BF3905_set_anti_flicker,//PNULL,//_BF3905_set_anti_flicker,
	PNULL,
	PNULL,//_BF3905_pick_out_jpeg_stream,
	PNULL, //meter_mode
	PNULL, //get_status
	_BF3905_StreamOn,
	_BF3905_StreamOff,
	PNULL,
};

/**---------------------------------------------------------------------------*
 **                         Global Variables                                  *
 **---------------------------------------------------------------------------*/


SENSOR_INFO_T g_BF3905_mipi_yuv_info =
{
	BF3905_I2C_ADDR_W,				// salve i2c write address
	BF3905_I2C_ADDR_R, 				// salve i2c read address
	0,								// bit0: 0: i2c register value is 8 bit, 1: i2c register value is 16 bit
									// bit2: 0: i2c register addr  is 8 bit, 1: i2c register addr  is 16 bit
									// other bit: reseved
	SENSOR_HW_SIGNAL_PCLK_P|\
	SENSOR_HW_SIGNAL_VSYNC_P|\
	SENSOR_HW_SIGNAL_HSYNC_P,		// bit0: 0:negative; 1:positive -> polarily of pixel clock
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
	SENSOR_LOW_PULSE_RESET,			// reset pulse level
	100,								// reset pulse width(ms)
	SENSOR_HIGH_LEVEL_PWDN,			// 1: high level valid; 0: low level valid
	2,									// count of identify code
	{{0xFC, 0x39},						// supply two code to identify sensor.
	{0xFD, 0x05}},						// for Example: index = 0-> Device id, index = 1 -> version id
	SENSOR_AVDD_2800MV,				// voltage of avdd
	640,								// max width of source image
	480,								// max height of source image
	"BF3905",							// name of sensor
	SENSOR_IMAGE_FORMAT_YUV422,		// define in SENSOR_IMAGE_FORMAT_E enum,SENSOR_IMAGE_FORMAT_MAX
										// if set to SENSOR_IMAGE_FORMAT_MAX here, image format depent on SENSOR_REG_TAB_INFO_T
	SENSOR_IMAGE_PATTERN_YUV422_UYVY,// pattern of input image form sensor; YUYV
	s_BF3905_resolution_Tab_YUV,			// point to resolution table information structure
	&s_BF3905_ioctl_func_tab,				// point to ioctl function table
	PNULL,								// information and table about Rawrgb sensor
	PNULL,								// extend information about sensor
	SENSOR_AVDD_1800MV,                    		// iovdd
	SENSOR_AVDD_1800MV,                   		 // dvdd
	3,                    							 // skip frame num before preview
	3,                      						// skip frame num before capture
	0,
	0,
	0,
	0,
	0,
	0,                     // threshold end postion
	0,                    // i2c_dev_handler
       {SENSOR_INTERFACE_TYPE_CSI2, 1, 8, 1},
	PNULL,
	3,
};

LOCAL void BF3905_WriteReg( uint8_t  subaddr, uint8_t data )
{
	
	#ifndef	_USE_DSP_I2C_
	
		Sensor_WriteReg_8bits(subaddr, data);

	#else
		DSENSOR_IICWrite((uint16_t)subaddr, (uint16_t)data);
	#endif


}

LOCAL uint8_t BF3905_ReadReg( uint8_t  subaddr)
{
	uint8_t value = 0;
	
	#ifndef	_USE_DSP_I2C_

	value = Sensor_ReadReg( subaddr);
	#else
		value = (uint16_t)DSENSOR_IICRead((uint16_t)subaddr);
	#endif

    
	return value;
}
LOCAL uint32_t _BF3905_GetResolutionTrimTab(uint32_t param)
{
    return (uint32_t)s_BF3905_Resolution_Trim_Tab;
}
/******************************************************************************/
// Description: sensor BF3905 power on/down sequence
// Global resource dependence: 
// Author: Tim.zhu
// Note:
/******************************************************************************/
LOCAL uint32_t _BF3905_PowerOn(uint32_t power_on)
{
	SENSOR_AVDD_VAL_E dvdd_val=g_BF3905_mipi_yuv_info.dvdd_val;
	SENSOR_AVDD_VAL_E avdd_val=g_BF3905_mipi_yuv_info.avdd_val;
	SENSOR_AVDD_VAL_E iovdd_val=g_BF3905_mipi_yuv_info.iovdd_val;  
	BOOLEAN power_down=g_BF3905_mipi_yuv_info.power_down_level;        
	BOOLEAN reset_level=g_BF3905_mipi_yuv_info.reset_pulse_level;
	//uint32_t reset_width=g_BF3905_mipi_yuv_info.reset_pulse_width;        

	if(SENSOR_TRUE==power_on)
	{
		Sensor_PowerDown(power_down);	
		// Open power
		Sensor_SetVoltage(dvdd_val, avdd_val, iovdd_val);    
		SENSOR_Sleep(20);
		Sensor_SetMCLK(SENSOR_DEFALUT_MCLK); 
		SENSOR_Sleep(10);
		Sensor_PowerDown(!power_down);
		// Reset sensor		
		Sensor_Reset(reset_level);	
		CMR_LOGE("qzhu BF3905 power_on is true\n");	
	}
	else
	{
		Sensor_PowerDown(power_down);
 SENSOR_Sleep(10);
		Sensor_SetMCLK(SENSOR_DISABLE_MCLK);         
		Sensor_SetVoltage(SENSOR_AVDD_CLOSED, SENSOR_AVDD_CLOSED, SENSOR_AVDD_CLOSED);   
		CMR_LOGE("qzhu BF3905 power_on is false\n");	     
	}    

	return SENSOR_SUCCESS;
}

/******************************************************************************/
// Description:
// Global resource dependence: 
// Author: Tim.zhu
// Note:
//        
/******************************************************************************/
LOCAL uint32_t _BF3905_Identify(uint32_t param)
{
#define BF3905_PID_VALUE    0x39    
#define BF3905_PID_ADDR     0xFC
#define BF3905_VER_VALUE    0x05    
#define BF3905_VER_ADDR     0xFD

	uint8_t pid_value=0x00;
	uint8_t ver_value=0x00;
	uint32_t ret_value=SENSOR_FAIL;


	CMR_LOGE("SENSOR:BF3905 identify  .\n");

	pid_value = Sensor_ReadReg(BF3905_PID_ADDR);

	if(BF3905_PID_VALUE==pid_value)
	{
		ver_value=Sensor_ReadReg(BF3905_VER_ADDR);

		CMR_LOGE("SENSOR: BF3905_Identify: PID = %x, VER = %x \n",pid_value, ver_value);

		if(BF3905_VER_VALUE==ver_value)
		{
			ret_value=SENSOR_SUCCESS;
			CMR_LOGE("SENSOR: this is BF3905 sensor ! \n");        
		}
		else
		{
			CMR_LOGE("SENSOR: BF3905_Identify this is TOSHIBA sensor ! \n");
		}
	}
	else
	{
		CMR_LOGE("SENSOR:BF3905 identify fail,pid_value=%d .\n",pid_value);
	}

	return ret_value;
}

LOCAL void BF3905_set_shutter()
{
	uint16_t temp_reg = 0;
	uint16_t shutter=0;

	//temp_reg = BF3905_ReadReg(0x13);
	//BF3905_WriteReg(0x13, (temp_reg&(~0x01))); /* Turn OFF AEC/AGC*/
	BF3905_WriteReg(0x13,0x02); 
	shutter = ((BF3905_ReadReg(0x8c) & 0xff) << 8) | (BF3905_ReadReg(0x8d));

	BF3905_WriteReg(0x1b,0x2e);

	//shutter=shutter/2;
	if(shutter<1)
	shutter=1;
	BF3905_WriteReg(0x8c, (shutter & 0xFF00) >> 8);
	BF3905_WriteReg(0x8D, shutter & 0xFF);

}
/******************************************************************************/
// Description: set brightness 
// Global resource dependence: 
// Author: Tim.zhu
// Note:
//
/******************************************************************************/

LOCAL const SENSOR_REG_T BF3905_brightness_tab[][2]=
{

	{{0x24, 0x30}, {0xff,0xff}},
	{{0x24, 0x38}, {0xff,0xff}},
	{{0x24, 0x40}, {0xff,0xff}},
	{{0x24, 0x40}, {0xff,0xff}},
	{{0x24, 0x50}, {0xff,0xff}},
	{{0x24, 0x58}, {0xff,0xff}},
	{{0x24, 0x60}, {0xff,0xff}},

};
LOCAL uint32_t _BF3905_set_brightness(uint32_t level)
{
	uint16_t i=0x00;
	uint32_t reg_bits = 0;
	uint32_t reg_value = 0;
            
	SENSOR_REG_T* sensor_reg_ptr=(SENSOR_REG_T*)BF3905_brightness_tab[level];

	if(level>6)
		return 0;

	for(i=0x00; (0xff!=sensor_reg_ptr[i].reg_addr)||(0xff!=sensor_reg_ptr[i].reg_value); i++)
	{
			Sensor_WriteReg(sensor_reg_ptr[i].reg_addr, sensor_reg_ptr[i].reg_value);
	}

	SENSOR_PRINT("SENSOR: _BF3905_set_brightness = 0x%02x,data=0x%x .\n", level,reg_value);
	return 0;
}

/******************************************************************************/
// Description: set contrast
// Global resource dependence: 
// Author: Tim.zhu
// Note:
//
/******************************************************************************/

LOCAL const SENSOR_REG_T BF3905_contrast_tab[][2]=
{
	      {//level 1
		{0x56,0x28}, {0xff,0xff}//-3
		},
		{//level 2
		 {0x56,0x30}, {0xff,0xff}
		},
		{//level 3
		{0x56,0x38}, {0xff,0xff}
		},
		{//level 4
		{0x56,0x3c}, {0xff,0xff}
		},
		{//level 5
		{0x56,0x48}, {0xff,0xff}
		},
		{//level 6
		{0x56,0x50}, {0xff,0xff}
		},
		{//level 7
		 {0x56,0x58}, {0xff,0xff}//+3
		},

};

LOCAL uint32_t _BF3905_set_contrast(uint32_t level)
{
	uint16_t i=0x00;
	uint32_t reg_bits = 0;
	uint32_t reg_value = 0;

        
	SENSOR_REG_T* sensor_reg_ptr=(SENSOR_REG_T*)BF3905_contrast_tab[level];
        
	if(level>6)
		return 0;

	for(i=0x00; (0xff!=sensor_reg_ptr[i].reg_addr)||(0xFF!=sensor_reg_ptr[i].reg_value); i++)
	{
			Sensor_WriteReg(sensor_reg_ptr[i].reg_addr, sensor_reg_ptr[i].reg_value);
	}

	//Sensor_SetSensorExifInfo(SENSOR_EXIF_CTRL_CONTRAST, (uint32)level);

	SENSOR_PRINT("SENSOR: _BF3905_set_contrast = 0x%02x,data=0x%x .\n", level,reg_value);
	return 0;
}
LOCAL const SENSOR_REG_T BF3905_anti_banding_flicker_tab[][3]=
{
    // 50HZ
    {
		{0x80, 0x92},///0904  0x9e
		{0x8a, 0x4c},///0xbf
		{0xff, 0xff}
    },
    //60HZ
    {
		{0x80, 0x90},
		{0x8b, 0x3f},
		{0xff, 0xff}
    },
};    

LOCAL uint32_t _BF3905_set_anti_flicker(uint32_t mode)
{

        
	SENSOR_REG_T_PTR sensor_reg_ptr=(SENSOR_REG_T_PTR)BF3905_anti_banding_flicker_tab[mode];
	uint16_t i=0x00;
        
	for(i=0x0; (0xff!=sensor_reg_ptr[i].reg_addr)||(0xff!=sensor_reg_ptr[i].reg_value); i++)
	{
		Sensor_WriteReg(sensor_reg_ptr[i].reg_addr, sensor_reg_ptr[i].reg_value);
	}

	SENSOR_PRINT("SENSOR: _BF3905_set_anti_flicker = 0x%02x \n", mode);

	return 0;
}
LOCAL uint32_t _BF3905_set_video_mode(uint32_t mode)
{
    uint16_t i;
    SENSOR_REG_T* sensor_reg_ptr = PNULL;
    uint8_t tempregval = 0;

    SENSOR_TRACE("SENSOR: set_video_mode: mode = %d\n", mode);
    return 0;
}

/******************************************************************************/
// Description:
// Global resource dependence: 
// Author: Tim.zhu
// Note:
//        
/******************************************************************************/
#if 0
LOCAL const SENSOR_REG_T BF3905_saturation_tab[][3]=
{
	{	//Saturation -2  
	},
	{	//Saturation -1  
	},
	{	//Saturation 0
	},
	{	//Saturation 1
	},
	{	//Saturation 2
	}
};
#endif
#if 0

LOCAL uint32_t _BF3905_set_saturation(uint32_t level)
{
	uint16_t i=0x00;
	uint32_t reg_bits = 0;
	uint32_t reg_value = 0;
        

        
	SENSOR_REG_T* sensor_reg_ptr=(SENSOR_REG_T*)BF3905_saturation_tab[level];
        
	if(level>6)
		return 0;

	for(i=0x0; (0xffff!=sensor_reg_ptr[i].reg_addr)||(0xFF!=sensor_reg_ptr[i].reg_value); i++)
	{
			Sensor_WriteReg(sensor_reg_ptr[i].reg_addr, sensor_reg_ptr[i].reg_value);
	}
	SENSOR_PRINT("SENSOR: _BF3905_set_saturation = 0x%02x,data=0x%x .\n", level,reg_value);
	return 0;
}
#endif

/******************************************************************************/
// Description:
// Global resource dependence: 
// Author: Tim.zhu
// Note:
//        
/******************************************************************************/

LOCAL const SENSOR_REG_T BF3905_image_effect_tab[][7]=
{
	// effect normal
	{
	 {0x70,0x0b},
	 {0x69,0x00},
	 {0x67,0x80},
	 {0x68,0x80},
	 {0xb4,0x63}, //03
	 {0x56,0x3c},
	 {0xff,0xff}
        },
	//effect BLACKWHITE
	{
	#if 1
	 {0x70,0x0b},
	 {0x69,0x20},
	 {0x67,0x80},
	 {0x68,0x80},
	 {0xb4,0x63}, 
	 {0x56,0x3c},
	 {0xff,0xff}
	 #endif

	 	  
},
	//  effect RED(don't use)
	{
	#if 1
	 {0x70,0x0b},
	 {0x69,0x20},
	 {0x67,0x80},
	 {0x68,0xc0},
	 {0xb4,0x63}, 
	 {0x56,0x3c},
	 {0xff,0xff}
	 #endif
},
	// effect GREEN(don't use)
	{
	#if 1
	 {0x70,0x0b},
	 {0x69,0x20},
	 {0x67,0x60},
	 {0x68,0x70},
	 {0xb4,0x63}, 
	 {0x56,0x3c},
	 {0xff,0xff}
	 #endif
},
	// effect  BLUE(don't use)
	{
	#if 1
	 {0x70,0x0b},
	 {0x69,0x20},
	 {0x67,0xe0},
	 {0x68,0x60},
	 {0xb4,0x63}, 
	 {0x56,0x3c},
	 {0xff,0xff}
	 #endif
},
	// effect  YELLOW(don't use)
	{
	#if 1
	 {0x70,0x0b},
	 {0x69,0x20},
	 {0x67,0x60},
	 {0x68,0xa0},
	 {0xb4,0x63}, 
	 {0x56,0x3c},
	 {0xff,0xff}
	 #endif
},
	// effect NEGATIVE
	{
	#if 1
	 {0x70,0x0b},
	 {0x69,0x21},
	 {0x67,0x80},
	 {0x68,0x80},
	 {0xb4,0x63}, 
	 {0x56,0x3c},
	 {0xff,0xff}
	 #endif
},
	//SEPIA
	{
	#if 1
	 {0x70,0x0b},
	 {0x69,0x20},
	 {0x67,0x60},
	 {0x68,0xa0},
	 {0xb4,0x63}, 
	 {0x56,0x3c},
	 {0xff,0xff}
	 #endif
}
};

LOCAL uint32_t _BF3905_set_image_effect(uint32_t effect_type)
{
	uint16_t i=0x00;
	uint32_t reg_bits = 0;
	uint32_t reg_value = 0;

        
	SENSOR_REG_T* sensor_reg_ptr=(SENSOR_REG_T*)BF3905_image_effect_tab[effect_type];
        
	if(effect_type>7)
		return 0;

	for(i=0x00; (0xff!=sensor_reg_ptr[i].reg_addr)||(0xff!=sensor_reg_ptr[i].reg_value); i++)
	{

			Sensor_WriteReg(sensor_reg_ptr[i].reg_addr, sensor_reg_ptr[i].reg_value);

	}

	SENSOR_PRINT("SENSOR: _BF3905_set_image_effect = 0x%02x,data=0x%x \n", effect_type,reg_value);
	return 0;
}

/******************************************************************************/
// Description:
// Global resource dependence: 
// Author: Tim.zhu
// Note:
//        
/******************************************************************************/
LOCAL const SENSOR_REG_T BF3905_ev_tab[][2]=
{
	{{0x55, 0xb0},  {0xff, 0xff}},
	{{0x55, 0xa0},  {0xff, 0xff}},
	{{0x55, 0x90},  {0xff, 0xff}},
	{{0x55, 0x85},  {0xff, 0xff}},
	{{0x55, 0x10},  {0xff, 0xff}},
	{{0x55, 0x20},  {0xff, 0xff}},
	{{0x55, 0x30},  {0xff, 0xff}}
};

LOCAL uint32_t _BF3905_set_ev(uint32_t level)
{

        
	SENSOR_REG_T_PTR sensor_reg_ptr=(SENSOR_REG_T_PTR)BF3905_ev_tab[level];
	uint16_t i=0x00;
        
	if(level>6)
		return 0;

	for(i=0x00; (0xff!=sensor_reg_ptr[i].reg_addr)||(0xFF!=sensor_reg_ptr[i].reg_value); i++)
	{
		Sensor_WriteReg(sensor_reg_ptr[i].reg_addr, sensor_reg_ptr[i].reg_value);
	}

	SENSOR_PRINT("SENSOR: _BF3905_set_ev = 0x%02x \n", level);
	return 0;
}


LOCAL const SENSOR_REG_T BF3905_awb_tab[][5] = 
{
	//AUTO
	{
	#if 1
	{0x13,0x07},
	{0x23,0x55},
//	{0x01,0x15},
//	{0x02,0x23},
	{0xff,0xff}
	#endif
	
},  
           //incandescent
	{
       #if 1
       {0x13,0x05},
	{0x23,0x33},
	{0x01,0x1f},
	{0x02,0x15},
	{0xff,0xff}
	#endif
}, 
           //cloudy
	{
	#if 1
    	{0x13,0x05},
	{0x23,0x33},
	{0x01,0x10},
	{0x02,0x28},
	{0xff,0xff}
	#endif 
}, 
           //daylight
	{
	#if 1
       {0x13,0x05},
	{0x23,0x33},
	{0x01,0x11},
	{0x02,0x26},
	{0xff,0xff}
	#endif 

},
	//fluoresent
	{
       #if 1 
       {0x13,0x05},
	{0x23,0x33},
	{0x01,0x1a},
	{0x02,0x1e},
	{0xff,0xff}
	#endif
}
};

LOCAL uint32_t _BF3905_set_awb(uint32_t mode)
{
	uint16_t i=0x00;
	uint32_t reg_bits = 0;
	uint32_t reg_value = 0;
        

        
	SENSOR_REG_T* sensor_reg_ptr=(SENSOR_REG_T*)BF3905_awb_tab[mode];

        
          if(mode>4)
		return 0;
		  
	for(i=0; (0xff!=sensor_reg_ptr[i].reg_addr)||(0xFF!=sensor_reg_ptr[i].reg_value); i++)
	{
			Sensor_WriteReg(sensor_reg_ptr[i].reg_addr, sensor_reg_ptr[i].reg_value);
	}

	SENSOR_PRINT("SENSOR: _BF3905_set_awb = 0x%02x,reg_value=0x%x \n", mode,reg_value);
	return 0;
}

/******************************************************************************/
// Description:
// Global resource dependence: 
// Author: Tim.zhu
// Note:
//        mode 0:normal;   1:night 
/******************************************************************************/
;

LOCAL uint32_t _BF3905_set_preview_mode(uint32_t preview_mode)
{
	SENSOR_PRINT("set_preview_mode: preview_mode = %d\n", preview_mode);
	
	//_BF3A20_set_anti_flicker(0);
	switch (preview_mode)
	{
		case DCAMERA_ENVIRONMENT_NORMAL: 
		{
	
						
			//YCP_saturation
			BF3905_WriteReg(0x89 , 0x65);  //7d
			BF3905_WriteReg(0x86 , 0x80);

				SENSOR_PRINT("set_preview_mode: DCAMERA_ENVIRONMENT_NORMAL\n");
			break;
		}
		case 1://DCAMERA_ENVIRONMENT_NIGHT://1
		{
			//YCP_saturation
			BF3905_WriteReg(0x89 , 0x65);  //7d
			BF3905_WriteReg(0x86 , 0x80);
				SENSOR_PRINT("set_preview_mode: DCAMERA_ENVIRONMENT_NIGHT\n");
				break;
		}
		case 3://SENSOR_ENVIROMENT_PORTRAIT://3
		{
			//YCP_saturation
			BF3905_WriteReg(0x89 , 0x65);  //7d
			BF3905_WriteReg(0x86 , 0x80);
				SENSOR_PRINT("set_preview_mode: SENSOR_ENVIROMENT_PORTRAIT\n");
				break;
		}
		case 4://SENSOR_ENVIROMENT_LANDSCAPE://4
		{
				//nightmode disable
			BF3905_WriteReg(0x89 , 0x65);   //7d
			BF3905_WriteReg(0x86 , 0x80);
				SENSOR_PRINT("set_preview_mode: SENSOR_ENVIROMENT_LANDSCAPE\n");
				break;
		}
		case 2://SENSOR_ENVIROMENT_SPORTS://2
		{
			//nightmode disable
			//YCP_saturation
			BF3905_WriteReg(0x89 , 0x65);  //7d
			BF3905_WriteReg(0x86 , 0x80);
				SENSOR_PRINT("set_preview_mode: SENSOR_ENVIROMENT_SPORTS\n");
				break;
		}
		default:
		{
			break;
		}
			
	}
	
	SENSOR_Sleep(20);
	
	return 0;
}

/******************************************************************************/
// Description:
// Global resource dependence: 
// Author: Tim.zhu
// Note:
//        
/******************************************************************************/

LOCAL uint32_t _BF3905_After_Snapshot(uint32_t param)
{
	SENSOR_PRINT("SENSOR: _BF3905_After_Snapshot =%d \n",param); 
        
  
        
    //   BF3905_WriteReg(0x1b,0x2e);
       BF3905_WriteReg(0x13,0x07);
	SENSOR_Sleep(100);
	Sensor_SetMode(param);

	return SENSOR_SUCCESS;
}


LOCAL uint32_t _BF3905_BeforeSnapshot(uint32_t sensor_snapshot_mode)
{		
     sensor_snapshot_mode &= 0xffff;
	 Sensor_SetMode(sensor_snapshot_mode);
	Sensor_SetMode_WaitDone();
	switch (sensor_snapshot_mode) {
	case SENSOR_MODE_PREVIEW_ONE:    //VGA
		SENSOR_PRINT("Capture VGA Size");
		 BF3905_WriteReg(0x13,0x02);
		SENSOR_Sleep(200);

		break;
	case SENSOR_MODE_SNAPSHOT_ONE_FIRST:  // 1.3 M
		{
	//	BF3905_set_shutter();	
	       BF3905_WriteReg(0x13,0x02);
		SENSOR_Sleep(450);  ///300
	//	SENSOR_PRINT("Capture 1.3M&2M Size");
		break; 
		}
	case SENSOR_MODE_SNAPSHOT_ONE_SECOND: // 2 M
		{
	//	BF3905_set_shutter();	
	       BF3905_WriteReg(0x13,0x02);
		SENSOR_Sleep(450);  ///300
	//	SENSOR_PRINT("Capture 1.3M&2M Size");
		break; 
		}

		
	default:
		break;
	}

	return 0;
}



/******************************************************************************/
// Description:
// Global resource dependence: 
// Author: Tim.zhu
// Note:
//        
/******************************************************************************/


LOCAL uint32_t _BF3905_StreamOn(uint32_t param)
{
	SENSOR_PRINT("SENSOR: _BF3905_StreamOn");
	int 	ret = 0;
	uint16_t     read_val = 0;
	ret = Sensor_WriteReg(0x09, 0x01);
		SENSOR_Sleep(20);
	
	//SENSOR_PRINT("SENSOR: read_BF3905, reg:0x3012  val: 0x%x.\n", read_val);
	return ret;
}

LOCAL uint32_t _BF3905_StreamOff(uint32_t param)
{
	SENSOR_PRINT("SENSOR: _BF3905_StreamOff");
	int 	ret = 0;
	ret = Sensor_WriteReg(0x09, 0x01);  //81
		SENSOR_Sleep(20);

	return ret;
}
/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/    
#ifdef __cplusplus
}
#endif

