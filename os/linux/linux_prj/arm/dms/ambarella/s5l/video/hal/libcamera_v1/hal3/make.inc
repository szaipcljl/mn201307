
ifeq ($(BUILD_WUJI_CAMERA_HAL), y)
LOCAL_PATH	:= $(call my-dir)

include $(CLEAR_VARS)
LIB_PATH := $(FAKEROOT_DIR)/usr/lib

##########################################################
##project RDA602
##########################################################
ifeq ($(CONFIG_WUJI_SYSTEM_HAL_DEFCONFIG_STRING), "RDA602")
LOCAL_SRCS		:= $(LOCAL_PATH)/src/hal_uart.c \
				   $(LOCAL_PATH)/src/hal_config_a1_rda602.c

endif
##########################################################
##project RDA600
##########################################################
ifeq ($(CONFIG_WUJI_SYSTEM_HAL_DEFCONFIG_STRING), "RDA600")
LOCAL_SRCS		:= $(LOCAL_PATH)/src/hal_uart.c \
				   $(LOCAL_PATH)/src/hal_config_a1.c

endif

##########################################################
##project SXQC1801
##########################################################
ifeq ($(CONFIG_WUJI_SYSTEM_HAL_DEFCONFIG_STRING), "SXQC1801")
PLATFORM :=s5l

endif

LOCAL_SRCS		:= $(LOCAL_PATH)/RoadCamera3Factory.cpp \
				   $(LOCAL_PATH)/RoadCamera3Hal.cpp

LOCAL_CFLAGS	:= -I$(LOCAL_PATH)/../../include \
					-I$(LOCAL_PATH)/../oem/inc

LOCAL_LDFLAGS	:= -lpthread -lrt

LOCAL_TARGET	:= libcamera_$(PLATFORM).so
LOCAL_SO_NAME = libcamera_$(PLATFORM).so
include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
	@cp -dpRf $< $(LIB_PATH)/
	$(call prebuild-links)
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))

endif
