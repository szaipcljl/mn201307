LOCAL_PATH	:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_TARGET	:= i2c-test
LOCAL_SRCS	:= $(LOCAL_PATH)/i2c-test.c

LOCAL_CFLAGS	:= -I$(PREBUILD_3RD_PARTY_DIR)/opencv/usr/include \
				   -I$(AMB_TOPDIR)/device/s32v/bsp/drivers/io/frame_io/include \
				   -I$(AMB_TOPDIR)/device/s32v/bsp/drivers/io/dcu/include \
				   -I$(AMB_TOPDIR)/device/s32v/bsp/drivers/io/sdi/include \
				   -I$(AMB_TOPDIR)/device/s32v/bsp/isp/inc \
				   -I$(AMB_TOPDIR)/device/s32v/bsp/include \
				   -I$(AMB_TOPDIR)/device/s32v/bsp/platform/s32_v234/ \
				   -I$(LOCAL_PATH)/src/


include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
#@mkdir -p $(UNIT_TEST_PATH)/
#@cp -dpRf $< $(UNIT_TEST_PATH)/
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))

