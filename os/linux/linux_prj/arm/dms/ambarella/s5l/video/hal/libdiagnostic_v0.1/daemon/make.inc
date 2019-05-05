##
## History:
##    2019/04/26 - [Ma Ning] Created file
##
##    Copyright (c) 2019 Roadefend, Inc.
##

ifeq ($(BUILD_DIAGNOSTIC_DAEMON), y)
LOCAL_PATH	:= $(call my-dir)

include $(CLEAR_VARS)

#LIB_PATH := $(FAKEROOT_DIR)/usr/lib


APP_PATH := $(FAKEROOT_DIR)/usr/bin
LOCAL_TARGET	:= diagnosticd

LOCAL_SRCS		:= 	$(LOCAL_PATH)/signal_handler.c \
					$(LOCAL_PATH)/main.c

LOCAL_CFLAGS	:= -I$(LOCAL_PATH)/
LOCAL_CFLAGS	:= -I$(LOCAL_PATH)/../include/ \
					-I$(LOCAL_PATH)/../../include

LOCAL_LIBS  :=  libfault_detector.so libcmr_guarder.so

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
