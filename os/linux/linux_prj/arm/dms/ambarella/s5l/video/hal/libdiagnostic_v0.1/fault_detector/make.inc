
ifeq ($(BUILD_CMR_FAULT_DETECTOR), y)
LOCAL_PATH	:= $(call my-dir)

include $(CLEAR_VARS)

LIB_PATH := $(FAKEROOT_DIR)/usr/lib

PLATFORM :=s5l

LOCAL_SRCS		:= $(LOCAL_PATH)/cmr_fault_detector.c \
					$(LOCAL_PATH)/cmr_ft_recorder.c \
					$(LOCAL_PATH)/../../utils/i2c_io_ops.c \
					$(LOCAL_PATH)/brg_dev_info.c \
					$(LOCAL_PATH)/cmr_dev_info.c

LOCAL_CFLAGS	:= -I$(LOCAL_PATH)/../include/ \
					-I$(LOCAL_PATH)/../../include

LOCAL_LDFLAGS	:= -lpthread -lrt

LOCAL_TARGET	:= libfault_detector.so
LOCAL_SO_NAME = libfault_detector.so

include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
	@cp -dpRf $< $(LIB_PATH)/
	$(call prebuild-links)
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))

endif
