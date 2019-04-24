ifeq ($(BUILD_WUJI_CAMERA_HAL), y)

LOCAL_PATH	:= $(call my-dir)
UNIT_TEST_PATH  := $(FAKEROOT_DIR)/unittest

include $(CLEAR_VARS)

LOCAL_TARGET	:= cmr-if-test
LOCAL_SRCS	:= $(LOCAL_PATH)/cmr_interface.c \
				$(LOCAL_PATH)/../utils/i2c_io_ops.c \
				$(LOCAL_PATH)/../test/cmr_if_test.c

LOCAL_CFLAGS	:= -I$(LOCAL_PATH)/../include/ \
					-I$(LOCAL_PATH)/../include/utils


include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
	#@mkdir -p $(UNIT_TEST_PATH)/
	@cp -dpRf $< $(UNIT_TEST_PATH)/
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))


###############################################


LIB_PATH := $(FAKEROOT_DIR)/usr/lib
PLATFORM :=s5l

LOCAL_SRCS		:= $(LOCAL_PATH)/cmr_interface.c \
					$(LOCAL_PATH)/../utils/i2c_io_ops.c

LOCAL_CFLAGS	:= -I$(LOCAL_PATH)/../include/ \
					-I$(LOCAL_PATH)/../include/utils

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
