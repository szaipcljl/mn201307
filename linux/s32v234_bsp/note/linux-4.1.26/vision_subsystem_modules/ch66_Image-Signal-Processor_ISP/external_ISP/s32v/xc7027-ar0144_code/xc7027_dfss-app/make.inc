LOCAL_PATH	:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_TARGET	:= xc7027_dfss_app
LOCAL_SRCS	:= $(LOCAL_PATH)/main.cpp \
			   $(LOCAL_PATH)/rda600_viu_simple.c

LOCAL_CFLAGS	:= -I$(PREBUILD_3RD_PARTY_DIR)/opencv/usr/include \
				   -I$(PREBUILD_3RD_PARTY_DIR)/tinyxml2/usr/include \
				   -I$(AMB_TOPDIR)/device/s32v/bsp/drivers/io/frame_io/include \
				   -I$(AMB_TOPDIR)/device/s32v/bsp/drivers/io/dcu/include \
				   -I$(AMB_TOPDIR)/device/s32v/bsp/drivers/io/sdi/include \
				   -I$(AMB_TOPDIR)/device/s32v/bsp/isp/inc \
				   -I$(AMB_TOPDIR)/device/s32v/bsp/include \
				   -I$(AMB_TOPDIR)/device/s32v/bsp/platform/s32_v234/ \
				   -I$(LOCAL_PATH)/src/ \
				   -I$(LOCAL_PATH)/../dfss-lib/usr/include \
				   -lstdc++

LOCAL_LDFLAGS	:= -L$(PREBUILD_3RD_PARTY_DIR)/opencv/usr/lib -lopencv_core \
				   -L$(AMB_TOPDIR)/device/s32v/bsp/isp/firmware/build-v234ce-gnu-linux-d/ -lsequencer  \
				   -L$(PREBUILD_3RD_PARTY_DIR)/tinyxml2/usr/lib -ltinyxml2 \
				   -L$(LOCAL_PATH)/../dfss-lib/usr/lib -ldfss -led2 -lfd2 -lopenblas \
				   -lpthread

LOCAL_LIBS	:= libsdi.a \
			   libframe_io.a \
			   libumat.a \
			   liboal.a \
			   liblog.a \
			   libfdmadrv.a \
			   libseqdrv.a \
			   libcsidrv.a \
			   libviudrv.a \
			   libcamdrv.a \
			   libh264encdrv.a \
			   libh264decdrv.a \
			   libjpegdecdrv.a

include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
#@mkdir -p $(UNIT_TEST_PATH)/
#@cp -dpRf $< $(UNIT_TEST_PATH)/
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))

