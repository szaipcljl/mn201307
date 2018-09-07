LOCAL_PATH	:= $(call my-dir)
INITRC_PATH:=$(LOCAL_PATH)/../rootdir
include $(CLEAR_VARS)

SBIN_PATH := $(FAKEROOT_DIR)/sbin
LOCAL_TARGET	:= init

LOCAL_SRCS		:= 	$(LOCAL_PATH)/bootchart.cpp\
					$(LOCAL_PATH)/builtins.cpp\
					$(LOCAL_PATH)/devices.cpp\
					$(LOCAL_PATH)/init.cpp\
					$(LOCAL_PATH)/init_parser.cpp\
					$(LOCAL_PATH)/init_parser_test.cpp\
					$(LOCAL_PATH)/keychords.cpp\
					$(LOCAL_PATH)/log.cpp\
					$(LOCAL_PATH)/parser.cpp\
					$(LOCAL_PATH)/property_service.cpp\
					$(LOCAL_PATH)/signal_handler.cpp\
					$(LOCAL_PATH)/ueventd.cpp\
					$(LOCAL_PATH)/ueventd_parser.cpp\
					$(LOCAL_PATH)/util.cpp\
					$(LOCAL_PATH)/util_test.cpp\
					$(LOCAL_PATH)/watchdogd.cpp\
                                        $(LOCAL_PATH)/initdependence.cpp\
                                        $(LOCAL_PATH)/uevent.cpp \
                        
				
                           

LOCAL_CFLAGS	:=  -I$(LOCAL_PATH)/\
		    -I$(LOCAL_PATH)/utils/\
                    -I$(LOCAL_PATH)/../libdevipc/include \
		    -I$(LOCAL_PATH)/../../../../device/s32v/hal/ \
		    -std=c++11 \


LOCAL_LIBS := \
    


#LOCAL_LDFLAGS	:= -lpthread -ldevipc 
#-lrt


include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
	@mkdir -p $(SBIN_PATH)/
	@cp -dpRf $< $(SBIN_PATH)/
	@cp $(INITRC_PATH)/init.rc $(FAKEROOT_DIR)/
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))

