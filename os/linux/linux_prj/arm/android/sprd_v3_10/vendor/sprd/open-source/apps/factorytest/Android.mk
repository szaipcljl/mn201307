ifneq ($(TARGET_SIMULATOR),true)
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
BOARD_PRODUCT_NAME:=$(TARGET_BOARD)
$(warning BOARD_PRODUCT_NAME = $(BOARD_PRODUCT_NAME))
commands_local_path := $(LOCAL_PATH)
ifeq ($(strip $(BOARD_USE_EMMC)),true)
LOCAL_CFLAGS += -DCONFIG_EMMC
endif

ifeq ($(strip $(TARGET_USERIMAGES_USE_UBIFS)),true)
LOCAL_CFLAGS += -DCONFIG_NAND
endif
ifeq ($(PRODUCT_WIFI_DEVICE),bcm)
LOCAL_CFLAGS += -DBCM
endif
ifeq ($(BOARD_SPRD_WCNBT_MARLIN),true)
LOCAL_CFLAGS += -DSPRD_WCNBT_MARLIN
endif
ifeq ($(PRODUCT_SDCARD),sp7731gea_qhd)
LOCAL_CFLAGS += -DGEASDCARD
endif

ifeq ($(TARGET_BOARD),sp9830aea_5m_h100)
LOCAL_CFLAGS += -DSP9830AEA
endif

ifeq ($(TARGET_BOARD), sp9830aea_5m_volte)
LOCAL_CFLAGS += -DSP9830AEA
endif

ifeq ($(TARGET_BOARD),sp9830aec_4m_h100)
LOCAL_CFLAGS += -DSP9830AEC
endif

ifeq ($(TARGET_BOARD),sp9830iec_4m_h100)
LOCAL_CFLAGS += -DSP9830IEC
endif

ifneq ($(filter sp9832a_3h10 sp9832iea_4m_volte sp9832iea_4m_h100, $(TARGET_BOARD)),)
LOCAL_CFLAGS += -DSP9832IEA
endif

ifeq ($(TARGET_BOARD),sp9830aef_5m_volte)
LOCAL_CFLAGS += -DSP9830AEF
endif

ifeq ($(TARGET_BOARD),sp9830aed_5m_h100)
LOCAL_CFLAGS += -DSP9830AED
endif

ifeq ($(TARGET_BOARD_NAME),sp7731)
LOCAL_CFLAGS += -DSP7731
endif

ifeq ($(TARGET_BOARD_NAME),sp9838)
LOCAL_CFLAGS += -DSP9838
endif

ifeq ($(TARGET_BOARD_NAME),sp7720)
LOCAL_CFLAGS += -DSP7720
endif

ifeq ($(SUPPORT_TEST_ITEM_LED),true)
LOCAL_CFLAGS += -DSUPPORT_TEST_ITEM_LED
endif

LOCAL_C_INCLUDES    +=  $(LOCAL_PATH) \
			$(LOCAL_PATH)/minui \
			$(LOCAL_PATH)/common	\
		   	$(LOCAL_PATH)/testitem  \
			$(LOCAL_PATH)/res \
			$(LOCAL_PATH)/sensor

LOCAL_C_INCLUDES    +=  external/tinyalsa/include

LOCAL_STATIC_LIBRARIES += libftminui libpixelflinger_static libcutils liblog
LOCAL_SHARED_LIBRARIES := libtinyalsa libhardware libdl libhardware_legacy libutils

#libasound libeng_audio_mode
LOCAL_SRC_FILES := factorytest.c  \
		eng_tok.c \
		ui.c \
		ui_touch.c \
		parse_conf.c \


LOCAL_SRC_FILES += $(call all-c-files-under, testitem)
ifeq ($(BOARD_SPRD_WCNBT_SR2351),true)
  LOCAL_CFLAGS += -DSPRD_WCNBT_SR2351
endif

ifdef BOARD_HAVE_ACC
LOCAL_CFLAGS        += -DBOARD_HAVE_ACC=\"$(BOARD_HAVE_ACC)\"
LOCAL_CFLAGS += -DBOARD_HAVE_ACC_$(BOARD_HAVE_ACC)
endif

ifdef BOARD_HAVE_PLS
LOCAL_CFLAGS        += -DBOARD_HAVE_PLS=\"$(BOARD_HAVE_PLS)\"
LOCAL_CFLAGS += -DBOARD_HAVE_PLS_$(BOARD_HAVE_PLS)
endif

LOCAL_MODULE := factorytest
LOCAL_MODULE_TAGS := optional
LOCAL_MULTILIB := 32

include $(BUILD_EXECUTABLE)
#include $(call all-makefiles-under,$(LOCAL_PATH))
include $(commands_local_path)/minui/Android.mk
include $(commands_local_path)/sensor/Android.mk
endif

