////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2014 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (??MStar Confidential Information??) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

/**
 *
 * @file    mstar_drv_platform_interface.c
 *
 * @brief   This file defines the interface of touch screen
 *
 *
 */

/*=============================================================*/
// INCLUDE FILE
/*=============================================================*/

#include "mstar_drv_platform_interface.h"
#include "mstar_drv_main.h"
#include "mstar_drv_ic_fw_porting_layer.h"
#include "mstar_drv_platform_porting_layer.h"
#include "mstar_drv_utility_adaption.h"
#include <linux/irq.h>  


#ifdef CONFIG_ENABLE_HOTKNOT
#include "mstar_drv_hotknot.h"
#endif //CONFIG_ENABLE_HOTKNOT

/*=============================================================*/
// EXTERN VARIABLE DECLARATION
/*=============================================================*/

#ifdef CONFIG_ENABLE_GESTURE_WAKEUP
extern u32 g_GestureWakeupMode[2];
extern u8 g_GestureWakeupFlag;

#ifdef CONFIG_ENABLE_GESTURE_DEBUG_MODE
extern u8 g_GestureDebugFlag;
extern u8 g_GestureDebugMode;
#endif //CONFIG_ENABLE_GESTURE_DEBUG_MODE

#if 1 
static struct class  *tp_gesture_class;
static struct device *tp_gesture_dev;
static ssize_t gesture_enable_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t gesture_enable_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size);

// Fixme;
static DEVICE_ATTR(enable, 4775, gesture_enable_show, gesture_enable_store);

u16 mstar_gesture_enable       =0x00; //0x3fff;// TP_GESTURE_OFF;
u8 mstar_gesture_id       = 0x00;

static ssize_t gesture_enable_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return (sprintf(buf, "mstar_gesture_enable = 0x%x! mstar_gesture_id=0x%x\n", mstar_gesture_enable,mstar_gesture_id));
}

static ssize_t gesture_enable_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	int ret       = 0;
	size_t on_off = simple_strtoul(buf, NULL, 10);

#if 1
	mstar_gesture_enable=on_off;
#else
	switch (on_off)
	{
	case 0x00:
		{
			mstar_gesture_enable = TP_GESTURE_DISABLE;
			break;
		}

	case 0x01:
		{
			mstar_gesture_enable = mstar_gesture_enable;
			break;
		}

	default:
		return -EINVAL;
	}
#endif
	return size;
}

#endif


#endif //CONFIG_ENABLE_GESTURE_WAKEUP

#ifdef TP_PROXIMITY_SENSOR 
static u8 is_tp_suspand=0;
extern int PROXIMITY_SWITCH;

extern int Mstar_TP_face_mode_switch(int on);
#endif //TP_PROXIMITY_SENSOR 

#ifdef CONFIG_ENABLE_PROXIMITY_DETECTION
extern u8 g_EnableTpProximity;
#endif //CONFIG_ENABLE_PROXIMITY_DETECTION

#ifdef CONFIG_ENABLE_GLOVE_MODE
extern u8 g_IsEnableGloveMode;
#endif //CONFIG_ENABLE_GLOVE_MODE

extern u8 g_IsUpdateFirmware;

extern struct input_dev *g_InputDevice;
extern struct i2c_client *g_I2cClient;

#ifdef CONFIG_ENABLE_HOTKNOT
extern u8 g_HotKnotState;
extern u32 SLAVE_I2C_ID_DWI2C;
#endif //CONFIG_ENABLE_HOTKNOT

#ifdef CONFIG_ENABLE_CHARGER_DETECTION
extern u8 g_ForceUpdate;
#endif //CONFIG_ENABLE_CHARGER_DETECTION

#ifdef CONFIG_ENABLE_ESD_PROTECTION
extern int g_IsEnableEsdCheck;
extern struct delayed_work g_EsdCheckWork;
extern struct workqueue_struct *g_EsdCheckWorkqueue;
#endif //CONFIG_ENABLE_ESD_PROTECTION

extern u8 IS_FIRMWARE_DATA_LOG_ENABLED;


/*=============================================================*/
// GLOBAL VARIABLE DEFINITION
/*=============================================================*/


/*=============================================================*/
// LOCAL VARIABLE DEFINITION
/*=============================================================*/

#ifdef CONFIG_ENABLE_HOTKNOT
static u8 _gAMStartCmd[4] = {HOTKNOT_SEND, ADAPTIVEMOD_BEGIN, 0, 0};
#endif //CONFIG_ENABLE_HOTKNOT

/*=============================================================*/
// GLOBAL FUNCTION DEFINITION
/*=============================================================*/

#ifdef CONFIG_ENABLE_NOTIFIER_FB
int MsDrvInterfaceTouchDeviceFbNotifierCallback(struct notifier_block *pSelf, unsigned long nEvent, void *pData)
{
    struct fb_event *pEventData = pData;
    int *pBlank;

    if (pEventData && pEventData->data && nEvent == FB_EVENT_BLANK)
    {
        pBlank = pEventData->data;

        if (*pBlank == FB_BLANK_UNBLANK)
        {
            DBG(&g_I2cClient->dev, "*** %s() TP Resume ***\n", __func__);

            if (g_IsUpdateFirmware != 0) // Check whether update frimware is finished
            {
                DBG(&g_I2cClient->dev, "Not allow to power on/off touch ic while update firmware.\n");
                return 0;
            }

#ifdef CONFIG_ENABLE_PROXIMITY_DETECTION
            if (g_EnableTpProximity == 1)
            {
                DBG(&g_I2cClient->dev, "g_EnableTpProximity = %d\n", g_EnableTpProximity);
                return 0;
            }
#endif //CONFIG_ENABLE_PROXIMITY_DETECTION
            
#ifdef CONFIG_ENABLE_GESTURE_WAKEUP
#ifdef CONFIG_ENABLE_HOTKNOT
            if (g_HotKnotState != HOTKNOT_BEFORE_TRANS_STATE && g_HotKnotState != HOTKNOT_TRANS_STATE && g_HotKnotState != HOTKNOT_AFTER_TRANS_STATE)
#endif //CONFIG_ENABLE_HOTKNOT
            {
#ifdef CONFIG_ENABLE_GESTURE_DEBUG_MODE
                if (g_GestureDebugMode == 1)
                {
                    DrvIcFwLyrCloseGestureDebugMode();
                }
#endif //CONFIG_ENABLE_GESTURE_DEBUG_MODE

                if (g_GestureWakeupFlag == 1)
                {
                    DrvIcFwLyrCloseGestureWakeup();
                }
                else
                {
                    DrvPlatformLyrEnableFingerTouchReport(); 
                }
            }
#ifdef CONFIG_ENABLE_HOTKNOT
            else    // Enable touch in hotknot transfer mode
            {
                DrvPlatformLyrEnableFingerTouchReport();     
            }
#endif //CONFIG_ENABLE_HOTKNOT
#endif //CONFIG_ENABLE_GESTURE_WAKEUP
    
#ifdef CONFIG_ENABLE_HOTKNOT
            if (g_HotKnotState != HOTKNOT_BEFORE_TRANS_STATE && g_HotKnotState != HOTKNOT_TRANS_STATE && g_HotKnotState != HOTKNOT_AFTER_TRANS_STATE)
#endif //CONFIG_ENABLE_HOTKNOT        
            {
#ifdef CONFIG_ENABLE_REGULATOR_POWER_ON
                DrvPlatformLyrTouchDeviceRegulatorPowerOn(true);
#endif //CONFIG_ENABLE_REGULATOR_POWER_ON               
                DrvPlatformLyrTouchDevicePowerOn(); 
            }   
    
#ifdef CONFIG_ENABLE_CHARGER_DETECTION 
            {
                u8 szChargerStatus[20] = {0};
     
                DrvCommonReadFile("/sys/class/power_supply/battery/status", szChargerStatus, 20);
            
                DBG(&g_I2cClient->dev, "*** Battery Status : %s ***\n", szChargerStatus);
            
                g_ForceUpdate = 1; // Set flag to force update charger status
                
                if (strstr(szChargerStatus, "Charging") != NULL || strstr(szChargerStatus, "Full") != NULL || strstr(szChargerStatus, "Fully charged") != NULL) // Charging
                {
                    DrvFwCtrlChargerDetection(1); // charger plug-in
                }
                else // Not charging
                {
                    DrvFwCtrlChargerDetection(0); // charger plug-out
                }

                g_ForceUpdate = 0; // Clear flag after force update charger status
            }           
#endif //CONFIG_ENABLE_CHARGER_DETECTION

#ifdef CONFIG_ENABLE_GLOVE_MODE
            if (g_IsEnableGloveMode == 1)
            {
                DrvIcFwLyrOpenGloveMode();
            }
#endif //CONFIG_ENABLE_GLOVE_MODE

            if (IS_FIRMWARE_DATA_LOG_ENABLED)    
            {
                DrvIcFwLyrRestoreFirmwareModeToLogDataMode(); // Mark this function call for avoiding device driver may spend longer time to resume from suspend state.
            } //IS_FIRMWARE_DATA_LOG_ENABLED

#ifndef CONFIG_ENABLE_GESTURE_WAKEUP
            DrvPlatformLyrEnableFingerTouchReport(); 
#endif //CONFIG_ENABLE_GESTURE_WAKEUP

#ifdef CONFIG_ENABLE_ESD_PROTECTION
            g_IsEnableEsdCheck = 1;
            queue_delayed_work(g_EsdCheckWorkqueue, &g_EsdCheckWork, ESD_PROTECT_CHECK_PERIOD);
#endif //CONFIG_ENABLE_ESD_PROTECTION
        }
        else if (*pBlank == FB_BLANK_POWERDOWN)
        {
            DBG(&g_I2cClient->dev, "*** %s() TP Suspend ***\n", __func__);
            
#ifdef CONFIG_ENABLE_ESD_PROTECTION
            g_IsEnableEsdCheck = 0;
            cancel_delayed_work_sync(&g_EsdCheckWork);
#endif //CONFIG_ENABLE_ESD_PROTECTION

            if (g_IsUpdateFirmware != 0) // Check whether update frimware is finished
            {
                DBG(&g_I2cClient->dev, "Not allow to power on/off touch ic while update firmware.\n");
                return 0;
            }

#ifdef CONFIG_ENABLE_PROXIMITY_DETECTION
            if (g_EnableTpProximity == 1)
            {
                DBG(&g_I2cClient->dev, "g_EnableTpProximity = %d\n", g_EnableTpProximity);
                return 0;
            }
#endif //CONFIG_ENABLE_PROXIMITY_DETECTION

#ifdef CONFIG_ENABLE_GESTURE_WAKEUP
#ifdef CONFIG_ENABLE_HOTKNOT
            if (g_HotKnotState != HOTKNOT_BEFORE_TRANS_STATE && g_HotKnotState != HOTKNOT_TRANS_STATE && g_HotKnotState != HOTKNOT_AFTER_TRANS_STATE)
#endif //CONFIG_ENABLE_HOTKNOT
            {
                if (g_GestureWakeupMode[0] != 0x00000000 || g_GestureWakeupMode[1] != 0x00000000)
                {
                    DrvIcFwLyrOpenGestureWakeup(&g_GestureWakeupMode[0]);
                    return 0;
                }
            }
#endif //CONFIG_ENABLE_GESTURE_WAKEUP

#ifdef CONFIG_ENABLE_HOTKNOT
            if (g_HotKnotState == HOTKNOT_BEFORE_TRANS_STATE || g_HotKnotState == HOTKNOT_TRANS_STATE || g_HotKnotState == HOTKNOT_AFTER_TRANS_STATE)
            {
                IicWriteData(SLAVE_I2C_ID_DWI2C, &_gAMStartCmd[0], 4); 
            }
#endif //CONFIG_ENABLE_HOTKNOT 

            DrvPlatformLyrFingerTouchReleased(0, 0, 0); // Send touch end for clearing point touch
            input_sync(g_InputDevice);

            DrvPlatformLyrDisableFingerTouchReport();

#ifdef CONFIG_ENABLE_HOTKNOT
            if (g_HotKnotState != HOTKNOT_BEFORE_TRANS_STATE && g_HotKnotState != HOTKNOT_TRANS_STATE && g_HotKnotState != HOTKNOT_AFTER_TRANS_STATE)
#endif //CONFIG_ENABLE_HOTKNOT        
            {
                DrvPlatformLyrTouchDevicePowerOff(); 
#ifdef CONFIG_ENABLE_REGULATOR_POWER_ON
                DrvPlatformLyrTouchDeviceRegulatorPowerOn(false);
#endif //CONFIG_ENABLE_REGULATOR_POWER_ON
            }    
        }
    }

    return 0;
}

#else

#ifdef CONFIG_PLATFORM_USE_ANDROID_SDK_6_UPWARD
void MsDrvInterfaceTouchDeviceSuspend(struct device *pDevice)
#else
void MsDrvInterfaceTouchDeviceSuspend(struct early_suspend *pSuspend)
#endif //CONFIG_PLATFORM_USE_ANDROID_SDK_6_UPWARD
{
    DBG(&g_I2cClient->dev, "*** %s() ***\n", __func__);

#ifdef CONFIG_ENABLE_ESD_PROTECTION
    g_IsEnableEsdCheck = 0;
    cancel_delayed_work_sync(&g_EsdCheckWork);
#endif //CONFIG_ENABLE_ESD_PROTECTION

    if (g_IsUpdateFirmware != 0) // Check whether update frimware is finished
    {
        DBG(&g_I2cClient->dev, "Not allow to power on/off touch ic while update firmware.\n");
        return;
    }

#ifdef TP_PROXIMITY_SENSOR 
    if (PROXIMITY_SWITCH)
    {
        printk("***%s(), msg2133a's proximity already open, so cann't suspend!\n", __func__);
	 return;
    }
#endif

#ifdef CONFIG_ENABLE_PROXIMITY_DETECTION
    if (g_EnableTpProximity == 1)
    {
        DBG(&g_I2cClient->dev, "g_EnableTpProximity = %d\n", g_EnableTpProximity);
        return;
    }
#endif //CONFIG_ENABLE_PROXIMITY_DETECTION

#ifdef CONFIG_ENABLE_GESTURE_WAKEUP
	if(mstar_gesture_enable !=0) 
	{
		printk("***enable mstar gesture!!***\n");
#ifdef CONFIG_ENABLE_HOTKNOT
    if (g_HotKnotState != HOTKNOT_BEFORE_TRANS_STATE && g_HotKnotState != HOTKNOT_TRANS_STATE && g_HotKnotState != HOTKNOT_AFTER_TRANS_STATE)
#endif //CONFIG_ENABLE_HOTKNOT
    {
        if (g_GestureWakeupMode[0] != 0x00000000 || g_GestureWakeupMode[1] != 0x00000000)
        {
            DrvIcFwLyrOpenGestureWakeup(&g_GestureWakeupMode[0]);
				enable_irq_wake(gpio_to_irq(MS_TS_MSG_IC_GPIO_INT));
				irq_set_irq_type(gpio_to_irq(MS_TS_MSG_IC_GPIO_INT), IRQF_TRIGGER_HIGH | IRQF_NO_SUSPEND);
				enable_irq(gpio_to_irq(MS_TS_MSG_IC_GPIO_INT));
            return;
			}
        }
    }
#endif //CONFIG_ENABLE_GESTURE_WAKEUP

#ifdef CONFIG_ENABLE_HOTKNOT
    if (g_HotKnotState == HOTKNOT_BEFORE_TRANS_STATE || g_HotKnotState == HOTKNOT_TRANS_STATE || g_HotKnotState == HOTKNOT_AFTER_TRANS_STATE)
    {
        IicWriteData(SLAVE_I2C_ID_DWI2C, &_gAMStartCmd[0], 4); 
    }
#endif //CONFIG_ENABLE_HOTKNOT  

    DrvPlatformLyrFingerTouchReleased(0, 0, 0); // Send touch end for clearing point touch
    input_sync(g_InputDevice);

    DrvPlatformLyrDisableFingerTouchReport();

#ifdef CONFIG_ENABLE_HOTKNOT
    if (g_HotKnotState != HOTKNOT_BEFORE_TRANS_STATE && g_HotKnotState != HOTKNOT_TRANS_STATE && g_HotKnotState != HOTKNOT_AFTER_TRANS_STATE)
#endif //CONFIG_ENABLE_HOTKNOT        
    {
        DrvPlatformLyrTouchDevicePowerOff(); 
#ifdef CONFIG_PLATFORM_USE_ANDROID_SDK_6_UPWARD
#ifdef CONFIG_ENABLE_REGULATOR_POWER_ON
        DrvPlatformLyrTouchDeviceRegulatorPowerOn(false);
#endif //CONFIG_ENABLE_REGULATOR_POWER_ON               
#endif //CONFIG_PLATFORM_USE_ANDROID_SDK_6_UPWARD
    }    
#ifdef TP_PROXIMITY_SENSOR 
	is_tp_suspand=1;
#endif
}

#ifdef CONFIG_PLATFORM_USE_ANDROID_SDK_6_UPWARD
void MsDrvInterfaceTouchDeviceResume(struct device *pDevice)
#else
void MsDrvInterfaceTouchDeviceResume(struct early_suspend *pSuspend)
#endif //CONFIG_PLATFORM_USE_ANDROID_SDK_6_UPWARD
{
    DBG(&g_I2cClient->dev, "*** %s() ***\n", __func__);

    if (g_IsUpdateFirmware != 0) // Check whether update frimware is finished
    {
        DBG(&g_I2cClient->dev, "Not allow to power on/off touch ic while update firmware.\n");
        return;
    }

#ifdef TP_PROXIMITY_SENSOR 
    printk("***%s(PROXIMITY_SWITCH=%d,is_tp_suspand=%d)***\n",__func__,PROXIMITY_SWITCH,is_tp_suspand);
    if ((PROXIMITY_SWITCH) && (is_tp_suspand==0) ) 
    {
        printk("***%s(), msg2133a's proximity already open, tp is not suspand,so donn't need resume!\n", __func__);
	 return;
    }
#endif

#ifdef CONFIG_ENABLE_PROXIMITY_DETECTION
    if (g_EnableTpProximity == 1)
    {
        DBG(&g_I2cClient->dev, "g_EnableTpProximity = %d\n", g_EnableTpProximity);
        return;
    }
#endif //CONFIG_ENABLE_PROXIMITY_DETECTION

#ifdef CONFIG_ENABLE_GESTURE_WAKEUP
	if(mstar_gesture_enable !=0) 
	{
#ifdef CONFIG_ENABLE_HOTKNOT
    if (g_HotKnotState != HOTKNOT_BEFORE_TRANS_STATE && g_HotKnotState != HOTKNOT_TRANS_STATE && g_HotKnotState != HOTKNOT_AFTER_TRANS_STATE)
#endif //CONFIG_ENABLE_HOTKNOT
    {
#ifdef CONFIG_ENABLE_GESTURE_DEBUG_MODE
        if (g_GestureDebugMode == 1)
        {
            DrvIcFwLyrCloseGestureDebugMode();
        }
#endif //CONFIG_ENABLE_GESTURE_DEBUG_MODE

        if (g_GestureWakeupFlag == 1)
        {
            DrvIcFwLyrCloseGestureWakeup();
        }
        else
        {
            DrvPlatformLyrEnableFingerTouchReport(); 
        }
    }
#ifdef CONFIG_ENABLE_HOTKNOT
    else    // Enable touch in hotknot transfer mode
    {
        DrvPlatformLyrEnableFingerTouchReport();     
    }
#endif //CONFIG_ENABLE_HOTKNOT
		disable_irq_wake(gpio_to_irq(MS_TS_MSG_IC_GPIO_INT));
		irq_set_irq_type(gpio_to_irq(MS_TS_MSG_IC_GPIO_INT),IRQF_TRIGGER_RISING);
	}
#endif //CONFIG_ENABLE_GESTURE_WAKEUP
    
#ifdef CONFIG_ENABLE_HOTKNOT
    if (g_HotKnotState != HOTKNOT_BEFORE_TRANS_STATE && g_HotKnotState != HOTKNOT_TRANS_STATE && g_HotKnotState != HOTKNOT_AFTER_TRANS_STATE)
#endif //CONFIG_ENABLE_HOTKNOT        
    {
#ifdef CONFIG_PLATFORM_USE_ANDROID_SDK_6_UPWARD
#ifdef CONFIG_ENABLE_REGULATOR_POWER_ON
        DrvPlatformLyrTouchDeviceRegulatorPowerOn(true);
#endif //CONFIG_ENABLE_REGULATOR_POWER_ON               
#endif //CONFIG_PLATFORM_USE_ANDROID_SDK_6_UPWARD
        DrvPlatformLyrTouchDevicePowerOn(); 
    }   
    
#ifdef CONFIG_ENABLE_CHARGER_DETECTION 
    {
        u8 szChargerStatus[20] = {0};
 
        DrvCommonReadFile("/sys/class/power_supply/battery/status", szChargerStatus, 20);
        
        DBG(&g_I2cClient->dev, "*** Battery Status : %s ***\n", szChargerStatus);
        
        g_ForceUpdate = 1; // Set flag to force update charger status

        if (strstr(szChargerStatus, "Charging") != NULL || strstr(szChargerStatus, "Full") != NULL || strstr(szChargerStatus, "Fully charged") != NULL) // Charging
        {
            DrvFwCtrlChargerDetection(1); // charger plug-in
        }
        else // Not charging
        {
            DrvFwCtrlChargerDetection(0); // charger plug-out
        }

        g_ForceUpdate = 0; // Clear flag after force update charger status
    }           
#endif //CONFIG_ENABLE_CHARGER_DETECTION

#ifdef CONFIG_ENABLE_GLOVE_MODE
    if (g_IsEnableGloveMode == 1)
    {
        DrvIcFwLyrOpenGloveMode();
    }
#endif //CONFIG_ENABLE_GLOVE_MODE

    if (IS_FIRMWARE_DATA_LOG_ENABLED)    
    {
        DrvIcFwLyrRestoreFirmwareModeToLogDataMode(); // Mark this function call for avoiding device driver may spend longer time to resume from suspend state.
    } //IS_FIRMWARE_DATA_LOG_ENABLED

	//#ifndef CONFIG_ENABLE_GESTURE_WAKEUP  /*关闭手势，TP唤醒无法使用*/
    DrvPlatformLyrEnableFingerTouchReport(); 
	//#endif //CONFIG_ENABLE_GESTURE_WAKEUP

#ifdef CONFIG_ENABLE_ESD_PROTECTION
    g_IsEnableEsdCheck = 1;
    queue_delayed_work(g_EsdCheckWorkqueue, &g_EsdCheckWork, ESD_PROTECT_CHECK_PERIOD);
#endif //CONFIG_ENABLE_ESD_PROTECTION

#ifdef TP_PROXIMITY_SENSOR 
	is_tp_suspand=0;

    printk("***%s(PROXIMITY_SWITCH=%d,is_tp_suspand=%d)***\n",__func__,PROXIMITY_SWITCH,is_tp_suspand);
    if ((PROXIMITY_SWITCH) && (is_tp_suspand==0) )
    {
         printk("***msg2133a's proximity re-open!\n");
	Mstar_TP_face_mode_switch(0);

	Mstar_TP_face_mode_switch(1);
    }
#endif

}

#endif //CONFIG_ENABLE_NOTIFIER_FB

/* probe function is used for matching and initializing input device */
s32 /*__devinit*/ MsDrvInterfaceTouchDeviceProbe(struct i2c_client *pClient, const struct i2c_device_id *pDeviceId)
{
    s32 nRetVal = 0;

    DBG(&g_I2cClient->dev, "*** %s() ***\n", __func__);
  
    DrvPlatformLyrVariableInitialize(); 

    DrvPlatformLyrTouchDeviceRequestGPIO(pClient);

#ifdef CONFIG_ENABLE_REGULATOR_POWER_ON
    DrvPlatformLyrTouchDeviceRegulatorPowerOn(true);
#endif //CONFIG_ENABLE_REGULATOR_POWER_ON

    DrvPlatformLyrTouchDevicePowerOn();

    nRetVal = DrvMainTouchDeviceInitialize();
    if (nRetVal == -ENODEV)
    {
        DrvPlatformLyrTouchDeviceRemove(pClient);
        return nRetVal;
    }

    DrvPlatformLyrInputDeviceInitialize(pClient); 
    
#ifdef CONFIG_ENABLE_GESTURE_WAKEUP 
	tp_gesture_class = class_create(THIS_MODULE, "ctp_class");
	if (IS_ERR(tp_gesture_class))
	{
		pr_err("Failed to create class(gesture)!\n");
		goto exit_fail_create_gesture_class;
	}

	tp_gesture_dev = device_create(tp_gesture_class, NULL, 0, NULL, "ctp_dev");
	if (IS_ERR(tp_gesture_dev))
	{
		pr_err("Failed to create device(gesture)!\n");
		goto exit_fail_create_gesture_device;
	}

	if (device_create_file(tp_gesture_dev, &dev_attr_enable) < 0)
	{
		pr_err("Failed to create file(gesture_store)!\n");
		goto exit_fail_create_gesture_device_file;
	}
#endif 	

#ifdef CONFIG_TOUCH_DRIVER_RUN_ON_MTK_PLATFORM
#ifdef CONFIG_ENABLE_DMA_IIC
    DmaAlloc(); // DmaAlloc() shall be called after DrvPlatformLyrInputDeviceInitialize()
#endif //CONFIG_ENABLE_DMA_IIC
#endif //CONFIG_TOUCH_DRIVER_RUN_ON_MTK_PLATFORM

    DrvPlatformLyrTouchDeviceRegisterFingerTouchInterruptHandler();

    DrvPlatformLyrTouchDeviceRegisterEarlySuspend();

#ifdef CONFIG_UPDATE_FIRMWARE_BY_SW_ID
    DrvIcFwLyrCheckFirmwareUpdateBySwId();
#endif //CONFIG_UPDATE_FIRMWARE_BY_SW_ID

#ifdef CONFIG_ENABLE_ESD_PROTECTION
    INIT_DELAYED_WORK(&g_EsdCheckWork, DrvPlatformLyrEsdCheck);
    g_EsdCheckWorkqueue = create_workqueue("esd_check");
    queue_delayed_work(g_EsdCheckWorkqueue, &g_EsdCheckWork, ESD_PROTECT_CHECK_PERIOD);
#endif //CONFIG_ENABLE_ESD_PROTECTION

    DBG(&g_I2cClient->dev, "*** MStar touch driver registered ***\n");
    
    return nRetVal;
#ifdef CONFIG_ENABLE_GESTURE_WAKEUP 
exit_fail_create_gesture_device_file:
	device_remove_file(tp_gesture_dev, &dev_attr_enable);

exit_fail_create_gesture_device:
	device_destroy(tp_gesture_class, tp_gesture_dev);
	tp_gesture_dev = NULL;

exit_fail_create_gesture_class:
	class_destroy(tp_gesture_class);
	tp_gesture_class = NULL;

	return -1;	
#endif 

}

/* remove function is triggered when the input device is removed from input sub-system */
s32 /*__devexit*/ MsDrvInterfaceTouchDeviceRemove(struct i2c_client *pClient)
{
    DBG(&g_I2cClient->dev, "*** %s() ***\n", __func__);

    return DrvPlatformLyrTouchDeviceRemove(pClient);
}

void MsDrvInterfaceTouchDeviceSetIicDataRate(struct i2c_client *pClient, u32 nIicDataRate)
{
    DBG(&g_I2cClient->dev, "*** %s() ***\n", __func__);

    DrvPlatformLyrSetIicDataRate(pClient, nIicDataRate);
}    
