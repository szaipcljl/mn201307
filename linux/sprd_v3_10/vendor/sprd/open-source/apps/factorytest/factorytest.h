#ifndef _FACTORY_H
#define _FACTORY_H

//#define SPRD_VIRTUAL_TOUCH
/**************Backlight************/
#define LCD_BACKLIGHT_DEV			"/sys/class/backlight/sprd_backlight/brightness"
#define KEY_BACKLIGHT_DEV 			"/sys/class/leds/keyboard-backlight/brightness"
#define LCD_BACKLIGHT_MAX_DEV		"/sys/class/backlight/sprd_backlight/max_brightness"
#define KEY_BACKLIGHT_MAX_DEV 		"/sys/class/leds/keyboard-backlight/max_brightness"
/*************end Backlight*********/

/************Bcamera************/
#define LIBRARY_PATH "/system/lib/hw/"
#define FLASH_SUPPORT "/sys/class/flash_test/flash_test/flash_value"
#define FLASH_YES_NO "flash_mode is :0x0"

/*********end Bcamera***********/

/************BT*************/
/************************************************************************************
 **  Constants & Macros
 ************************************************************************************/
#define SPRD_BT_TITLE_Y     40
#define SPRD_BT_TITLE       "SpreadTrum BT Test"

/* Common Bluetooth field definitions */
#define BT_MAC_CONFIG_FILE_PATH  "/data/misc/bluedroid/btmac.txt"
#define BT_MAC_FACTORY_CONFIG_FILE_PATH  "/productinfo/btmac.txt"
#define BD_ADDR_LEN     6                         /* Device address length */
#define BD_NAME_LEN     248                       /* Device name length */
typedef unsigned char BD_ADDR[BD_ADDR_LEN];       /* Device address */
typedef unsigned char*BD_ADDR_PTR;                /* Pointer to Device Address */
typedef unsigned char BD_NAME[BD_NAME_LEN + 1];   /* Device name */
typedef unsigned char*BD_NAME_PTR;                /* Pointer to Device name */
#define BT_MAC_STR_LEN     (17)
#define MAC_LEN     (6)
#define MAX_REMOTE_DEVICES (10)
#define CASE_RETURN_STR(const) case const: return #const;

//The time of BT Discovery
#define BT_DISCOVERY_TIME    2
#define BT_ENABLE_EVNET             1
#define BT_DISABLE_EVNET            2
#define BT_DISCOVERY_START_EVNET    3
#define BT_DISCOVERY_STOP_EVNET     4

/******************BT end***************/

/**************charge******************/
#define SPRD_CALI_MASK			0x00000200

#define ENG_BATVOL		"/sys/class/power_supply/battery/real_time_voltage"
#define ENG_CHRVOL		"/sys/class/power_supply/battery/charger_voltage"
#define ENG_CURRENT		"/sys/class/power_supply/battery/real_time_current"
#define ENG_USBONLINE	"/sys/class/power_supply/usb/online"
#define ENG_ACONLINE	"/sys/class/power_supply/ac/online"
#define ENG_BATONLINE   "/sys/class/power_supply/battery/health"
#define ENG_STOPCHG	    "/sys/class/power_supply/battery/stop_charge"
#define ENG_BATCURRENT  "/sys/class/power_supply/sprdfgu/fgu_current"
/****************end charge*************/

/***************factorytest.c************/
#define PCBATXTPATH "/productinfo/PCBAtest.txt"
#define PHONETXTPATH   "/productinfo/wholephonetest.txt"
/****************end*****************/

/**************FM******************/

#define FM_INSMOD_COMMEND    "insmod /system/lib/modules/trout_fm.ko"

#define FM_IOCTL_BASE     	  'R'
#define FM_IOCTL_ENABLE       _IOW(FM_IOCTL_BASE, 0, int)
#define FM_IOCTL_GET_ENABLE   _IOW(FM_IOCTL_BASE, 1, int)
#define FM_IOCTL_SET_TUNE     _IOW(FM_IOCTL_BASE, 2, int)
#define FM_IOCTL_GET_FREQ     _IOW(FM_IOCTL_BASE, 3, int)
#define FM_IOCTL_SEARCH       _IOW(FM_IOCTL_BASE, 4, int[4])
#define FM_IOCTL_STOP_SEARCH  _IOW(FM_IOCTL_BASE, 5, int)
#define FM_IOCTL_SET_VOLUME   _IOW(FM_IOCTL_BASE, 7, int)
#define FM_IOCTL_GET_VOLUME   _IOW(FM_IOCTL_BASE, 8, int)
#define Trout_FM_IOCTL_CONFIG _IOW(FM_IOCTL_BASE, 9, int)
#define FM_IOCTL_GET_RSSI     _IOW(FM_IOCTL_BASE, 10, int)


#define TROUT_FM_DEV_NAME   		"/dev/Trout_FM" //
//#define SPRD_HEADSET_SWITCH_DEV     "/sys/class/switch/h2w/state"
#define SPRD_HEADSETOUT             0
#define SPRD_HEADSETIN              1
//#define FM_CLOSE     				0
//#define FM_PLAY     				1
//#define FM_PLAY_ERR     			2
typedef enum
{
    HEADSET_CLOSE = 0,
    HEADSET_OPEN ,
    HEADSET_CHECK,
    HEADSET_INVALID
}HEADSET_CMD_TYPE;

#define STATE_CLEAN     			0
#define STATE_DISPLAY   			1
#define MAX_NAME_LEN 4096

#define V4L2_CID_PRIVATE_BASE           0x8000000
#define V4L2_CID_PRIVATE_TAVARUA_STATE  (V4L2_CID_PRIVATE_BASE + 4)

#define V4L2_CTRL_CLASS_USER            0x980000
#define V4L2_CID_BASE                   (V4L2_CTRL_CLASS_USER | 0x900)
#define V4L2_CID_AUDIO_VOLUME           (V4L2_CID_BASE + 5)
#define V4L2_CID_AUDIO_MUTE             (V4L2_CID_BASE + 9)


#define START_FRQ	10410
#define END_FRQ     8750
#define THRESH_HOLD 100
#define DIRECTION   128
#define SCANMODE    0
#define MUTI_CHANNEL false
#define CONTYPE     0
#define CONVALUE    0

/******************END*******************/

/****************GPS****************/
#define SPRD_GPS_TITLE_Y 	40
#define SPRD_GPS_TITLE 		"SpreadTrum GPS Test"
#ifdef GPS_DEBUG
#define FUN_ENTER	LOGD("%s enter \n",__FUNCTION__);
#define FUN_EXIT 	LOGD("%s exit \n",__FUNCTION__);
#else
#define FUN_ENTER
#define FUN_EXIT
#endif

#define GPSNATIVETEST_WAKE_LOCK_NAME  "gps_native_test"
#define GPS_TEST_PASS  TEXT_TEST_PASS   //"PASS"
#define GPS_TEST_FAILED  TEXT_TEST_FAIL //"FAILED"
#define GPS_TESTING  TEXT_BT_SCANING    //"TESTING......"
#define GPS_TEST_TIME_OUT	     (30) // s120
/********************end gps*************/

/*****************Gsensor*************/
#if defined(BOARD_HAVE_ACC_Mc3xxx) //huafeizhou160106 add
#define SPRD_GSENSOR_DEV					"/dev/mc3xxx"
#else
#define SPRD_GSENSOR_DEV					"/dev/lis3dh_acc"
#endif
#define	GSENSOR_IOCTL_BASE 77
#define GSENSOR_IOCTL_GET_XYZ           _IOW(GSENSOR_IOCTL_BASE, 22, int)
#define GSENSOR_IOCTL_SET_ENABLE      	_IOW(GSENSOR_IOCTL_BASE, 2, int)
#define GSENSOR_IOCTL_GET_CHIP_ID    _IOR(GSENSOR_IOCTL_BASE, 255, char[32])

#define SPRD_GSENSOR_OFFSET					60
#define SPRD_GSENSOR_1G						1024
#define GSENSOR_TIMEOUT                     20
/***************end gsensor************/

/****************Headerset***********/
#define SPRD_HEADSET_SWITCH_DEV 		"/sys/class/switch/h2w/state"
#define SPRD_HEASETKEY_DEV				"headset-keyboard"
#define SPRD_HEADSET_KEY	   			KEY_MEDIA
#define SPRD_HEADSET_KEYLONGPRESS		KEY_END
/**************end Headerset***********/

/***************key**************/
#define KEY_TIMEOUT 20
/***************end key**********/

/**************Lsensor**********/
#if !((defined SP7731) || (defined SP9830AEC) || (defined SP9830IEC) || (defined SP9832IEA))
#define SPRD_PLS_CTL				"/dev/ltr_558als"
#define SPRD_PLS_LIGHT_THRESHOLD	30
#define SPRD_PLS_INPUT_DEV			"alps_pxy"
#else
#if defined(BOARD_HAVE_PLS_EPL2182)
#define SPRD_PLS_CTL				"/dev/epl2182_pls"
#define SPRD_LS_CTL				"/dev/epl2182_pls"
#define SPRD_PLS_LIGHT_THRESHOLD	1
#define SPRD_LS_INPUT_DEV			"lightsensor-level"
#define SPRD_PS_INPUT_DEV			"proximity"
#elif defined(BOARD_HAVE_PLS_ELAN)
#define SPRD_LS_CTL				"/dev/elan_als"
#define SPRD_PS_CTL				"/dev/elan_ps"
#define SPRD_PLS_LIGHT_THRESHOLD	1
#define SPRD_LS_INPUT_DEV			"lightsensor-level"
#define SPRD_PS_INPUT_DEV			"proximity"
#elif defined(BOARD_HAVE_PLS_TP)
#define SPRD_PS_CTL				"/sys/board_properties/facemode"
#define SPRD_PS_INPUT_DEV			"focaltech_ts"
#else
#define SPRD_LS_CTL				"/dev/ltr_558als"
#define SPRD_PLS_LIGHT_THRESHOLD	30
#define SPRD_LS_INPUT_DEV			"lightsensor-level"
#define SPRD_PS_INPUT_DEV			"proximity"
#endif
#endif

/**************TP**********/
#if ((defined SP7720) || (defined SP9830AEA) || (defined SP9830AEF) || (defined SP9830AED))
#define SPRD_TS_INPUT_DEV			"msg2138_ts"
#else
#define SPRD_TS_INPUT_DEV			"focaltech_ts"
#endif
//add-s
#define DEVICE_HWINFO_PATH "/sys/board_properties/tp_information"

typedef struct {
	char* vendor;
	char* dev_name;
}tp_info;
//add-e

/**************Gsensor**********/
#define SPRD_ACC_INPUT_DEV			"accelerometer"

#if defined(BOARD_HAVE_PLS_ELAN)
#define LTR_IOCTL_MAGIC         'c'
#define LTR_IOCTL_GET_PFLAG	    _IOR(LTR_IOCTL_MAGIC, 1, int)
#define LTR_IOCTL_GET_LFLAG     _IOR(LTR_IOCTL_MAGIC, 2, int)
#define LTR_IOCTL_SET_PFLAG     _IOW(LTR_IOCTL_MAGIC, 3, int)
#define LTR_IOCTL_SET_LFLAG     _IOW(LTR_IOCTL_MAGIC, 4, int)
#define LTR_IOCTL_GET_DATA      _IOW(LTR_IOCTL_MAGIC, 5, unsigned char)
#else
#define LTR_IOCTL_MAGIC         0x1C
#define LTR_IOCTL_GET_PFLAG	    _IOR(LTR_IOCTL_MAGIC, 1, int)
#define LTR_IOCTL_GET_LFLAG     _IOR(LTR_IOCTL_MAGIC, 2, int)
#define LTR_IOCTL_SET_PFLAG     _IOW(LTR_IOCTL_MAGIC, 3, int)
#define LTR_IOCTL_SET_LFLAG     _IOW(LTR_IOCTL_MAGIC, 4, int)
#define LTR_IOCTL_GET_DATA      _IOW(LTR_IOCTL_MAGIC, 5, unsigned char)
#endif
/**************end Lsensor********/


/************modem*************/
static char* modem_w_port[] = {
	"/dev/stty_w1",
	"/dev/stty_w4"
};
static char* modem_lte_port[] = {
	"/dev/stty_lte1",
	"/dev/stty_lte4"
};

static char* tel_w_port[] = {
	"/dev/stty_w0",
	"/dev/stty_w3"
};

static char* tel_lte_port[] = {
	"/dev/stty_lte0",
	"/dev/stty_lte3"
};

#define PROP_MODEM_W_COUNT  "ro.modem.w.count"
#define PROP_MODEM_LTE_COUNT  "ro.modem.l.count"

#define TEL_DEVICE_W_PATH "/dev/stty_w2"
#define TEL_DEVICE_LTE_PATH "/dev/stty_lte2"

#define MAX_MODEM_COUNT 	2

#define SCREEN_TEXT_LEN		40
#define TITLE_Y_POS 		30
/*************end modem************/


/***************sdcard*************/
#ifdef CONFIG_NAND
#define SPRD_SD_DEV			"/dev/block/mmcblk0p1"
#define SPRD_SD_DEV_SIZE	"/sys/block/mmcblk1/size"
#define SPRD_MOUNT_DEV		"mount -t vfat /dev/block/mmcblk0p1  /sdcard"
#else
#define SPRD_SD_DEV			"/dev/block/mmcblk1"
#define SPRD_SD_DEV_SIZE	"/sys/block/mmcblk1/size"
#define SPRD_MOUNT_DEV		"mount -t vfat /dev/block/mmcblk1  /sdcard"
#endif
#define SPRD_UNMOUNT_DEV	"unmount /sdcard"

#define RW_LEN	512
/******************end sdcard*************/


/*****************speaker***************/

#define CARD_SPRDPHONE "sprdphone"

/***************end spreaker**************/

#define ABS_MT_POSITION_X			0x35	/* Center X ellipse position */
#define ABS_MT_POSITION_Y			0x36	/* Center Y ellipse position */
#define ABS_MT_TOUCH_MAJOR			0x30
#define SYN_REPORT					0
#define SYN_MT_REPORT				2
/****************end tp***************/

/**************UI*****************/
#define MAX_COLS 64
#define MAX_ROWS 48

#define CHAR_WIDTH 18
#ifdef SPRD_VIRTUAL_TOUCH
#define CHAR_HEIGHT 80
#else
#define CHAR_HEIGHT 35
#endif

#define PASS_POSITION (8*CHAR_HEIGHT)
#define FAIL_POSITION (16*CHAR_WIDTH)

#define PROGRESSBAR_INDETERMINATE_STATES 6
#define PROGRESSBAR_INDETERMINATE_FPS 15
/**************end UI**************/


/****************version**********/
#define PROP_ANDROID_VER	"ro.build.version.release"
#define PROP_SPRD_VER		"ro.build.description"
#define PROP_PRODUCT_VER	"ro.product.model"
#define PATH_LINUX_VER		"/proc/version"

/**************end version***********/


/***************vibrator***********/
#define VIBRATOR_ENABLE_DEV			"/sys/class/timed_output/vibrator/enable"
/*************end vibartor********/



/************wifi*****************/
#define WIFI_ADDRESS        "/sys/class/net/wlan0/address"
#define WIFI_MAC_CONFIG_FILE_PATH  "/data/misc/wifi/wifimac.txt"
#define WIFI_MAC_FACTORY_CONFIG_FILE_PATH  "/productinfo/wifimac.txt"

/****************OTG*************/
#define OTG_FILE_PATH  "/sys/bus/platform/drivers/dwc_otg/is_support_otg"
#define OTG_INSERT_STATUS "sys/devices/20200000.usb/otg_status"
#define OTG_DEVICE_HOST "sys/devices/20200000.usb/mode"
#define OTG_TESTFILE_PATH "/system/bin/test"
/****************end OTG**********/

/**************result***************/
typedef struct mmitest_result
{
    unsigned char type_id;
    unsigned char function_id;
    unsigned char eng_support;
    unsigned char pass_faild;   //0:not test,1:pass,2:fail
}mmi_result;

typedef struct mmi_show_data
{
	unsigned char id;
	char* name;
	int (*func)(void);
	mmi_result* mmi_result_ptr;
}mmi_show_data;

typedef struct hardware_result{
    char id;
    char support;
}hardware_result;


enum case_num
{
    CASE_TEST_LCD,
    CASE_TEST_TP,
    CASE_TEST_MULTITOUCH,
    CASE_TEST_KEY,
    CASE_TEST_VIBRATOR,
    CASE_TEST_BACKLIGHT,
    CASE_TEST_FCAMERA,
    CASE_TEST_BCAMERA,
    CASE_TEST_FLASH,
    CASE_TEST_MAINLOOP,
    CASE_TEST_ASSISLOOP,
    CASE_TEST_SPEAKER,
    CASE_TEST_RECEIVER,
    CASE_TEST_HEADSET,
    CASE_TEST_SDCARD,
    CASE_TEST_SIMCARD,
    CASE_TEST_CHARGE,
    CASE_TEST_WIRELESSCHARGER,
    CASE_TEST_FM,
    CASE_TEST_ATV,
    CASE_TEST_DTV,
    CASE_TEST_BT,
    CASE_TEST_WIFI,
    CASE_TEST_GPS,
    CASE_TEST_RTC,
    CASE_TEST_OTG,
    CASE_TEST_TEL,
    CASE_TEST_NFC,
    CASE_TEST_CALIBINFO,
    CASE_TEST_SOFTCHECK,
    CASE_TEST_IRREMOTE,
    CASE_TEST_ACCSOR,
    CASE_TEST_MAGSOR,
    CASE_TEST_ORISOR,
    CASE_TEST_GYRSOR,
    CASE_TEST_LPSOR,
    CASE_TEST_PRESSOR,
    CASE_TEST_TEMPESOR,
    CASE_TEST_GSENSOR,
    CASE_TEST_LSENSOR,
    CASE_TEST_RVSOR,
    CASE_TEST_FINGERSOR,
    CASE_TEST_HUMISOR,
    CASE_TEST_HALLSOR,
    CASE_TEST_LED,
    CASE_TEST_EMMC,
    CASE_TEST_RESERVED2,
    CASE_TEST_RESERVED3,
    CASE_TEST_RESERVED4,
    CASE_TEST_RESERVED5,
    CASE_TEST_RESERVED6,
    CASE_TEST_FORCUST,
    CASE_TEST_FORCUST1,
    CASE_TEST_FORCUST2,
    CASE_TEST_FORCUST3,
    CASE_TEST_FORCUST4,
    CASE_TEST_FORCUST5,
    CASE_TEST_FORCUST6,
    CASE_TEST_FORCUST7,
    CASE_TEST_FORCUST8,
    CASE_TEST_FORCUST9,
    CASE_TEST_FORCUST10,
    CASE_TEST_FORCUST11,
    FINAL_RESULT_FLAG,
    TOTAL_NUM
};

enum show_index
{
    CASE_LCD_INDEX,
    CASE_TP_INDEX,
    CASE_MULTI_TOUCH_INDEX,
    CASE_VB_BL_INDEX,
    CASE_LED_INDEX,
    CASE_KEY_INDEX,
    CASE_FCAMERA_INDEX,
    CASE_BCAMERA_INDEX,
    CASE_MAINLOOPBACK_INDEX,
    CASE_ASSILOOPBACK_INDEX,
    CASE_RECEIVER_INDEX,
    CASE_CHARGE_INDEX,
    CASE_SDCARD_INDEX,
    CASE_EMMC_INDEX,
    CASE_SIM_INDEX,
    CASE_RTC_INDEX,
    CASE_HEADSET_INDEX,
    CASE_FM_INDEX,
    CASE_GSENSOR_INDEX,
    CASE_LSENSOR_INDEX,
    CASE_BT_INDEX,
    CASE_WIFI_INDEX,
    CASE_GPS_INDEX,
    CASE_NUM
};

extern mmi_result phone_result[TOTAL_NUM];
extern mmi_result pcba_result[TOTAL_NUM];
extern hardware_result support_result[TOTAL_NUM];

/******************phase check*************/
#define  MAX_SN_LEN  24
#define SP09_MAX_SN_LEN   MAX_SN_LEN
#define SP09_MAX_STATION_NUM   15
#define SP09_MAX_STATION_NAME_LEN   10
#define SP09_SPPH_MAGIC_NUMBER   0x53503039
#define SP05_SPPH_MAGIC_NUMBER   0x53503035
#define SP09_MAX_LAST_DESCRIPTION_LEN   32

#define SN1_START_INDEX   4
#define SN2_START_INDEX  (SN1_START_INDEX + SP09_MAX_SN_LEN)

#define STATION_START_INDEX   56
#define TESTFLAG_START_INDEX  252
#define RESULT_START_INDEX   254

/****************phase check over*************/

#endif
