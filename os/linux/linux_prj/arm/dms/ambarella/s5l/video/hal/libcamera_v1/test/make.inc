
ifeq ($(BUILD_WUJI_CAMERA_HAL), y)
LOCAL_PATH	:= $(call my-dir)

include $(CLEAR_VARS)
APP_PATH := $(FAKEROOT_DIR)/usr/unittest


LOCAL_SRCS		:= $(LOCAL_PATH)/main.c

LOCAL_CFLAGS	:= 

LOCAL_LDFLAGS	:= -lpthread -lrt -ldl

LOCAL_TARGET	:= camera_app

include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)
$(LOCAL_TARGET): $(LOCAL_MODULE)
	@cp -dpRf $< $(APP_PATH)/
	$(call prebuild-links)
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))

endif
