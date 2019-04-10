
ifeq ($(BUILD_CMR_FAULT_DETECTOR), y)
LOCAL_PATH	:= $(call my-dir)

include $(CLEAR_VARS)

#LIB_PATH := $(FAKEROOT_DIR)/usr/lib

##########################################################
##project RDA602
##########################################################
ifeq ($(CONFIG_WUJI_SYSTEM_HAL_DEFCONFIG_STRING), "RDA602")
LOCAL_SRCS		:= $(LOCAL_PATH)/main.c
endif
##########################################################
##project RDA600
##########################################################
ifeq ($(CONFIG_WUJI_SYSTEM_HAL_DEFCONFIG_STRING), "RDA600")
LOCAL_SRCS		:= $(LOCAL_PATH)/main.c

endif

##########################################################
##project SXQC1801
##########################################################
ifeq ($(CONFIG_WUJI_SYSTEM_HAL_DEFCONFIG_STRING), "SXQC1801")
LOCAL_SRCS		:= $(LOCAL_PATH)/main.c

endif


APP_PATH := $(FAKEROOT_DIR)/usr/bin
LOCAL_TARGET	:= cmr_fault_detector

LOCAL_SRCS		:= $(LOCAL_PATH)/main.c $(LOCAL_PATH)/vin_cap.c \
				$(LOCAL_PATH)/cmr_fault_detector.c \
				$(LOCAL_PATH)/cmr_ft_recorder.c \
				$(LOCAL_PATH)/cmr_dev_info.c
#LOCAL_CFLAGS	:= I$(LOCAL_PATH)/

#LOCAL_LIBS  := 

LOCAL_LDFLAGS	:= -lpthread
#-lrt


include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
	@mkdir -p $(APP_PATH)/
	@cp -dpRf $< $(APP_PATH)/
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))
endif
