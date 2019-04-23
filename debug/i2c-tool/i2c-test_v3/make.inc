LOCAL_PATH	:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_TARGET	:= i2c-test
LOCAL_SRCS	:= $(LOCAL_PATH)/i2c-test.c \
				$(LOCAL_PATH)/i2c_io_ops.c

LOCAL_CFLAGS	:= -I$(LOCAL_PATH)/include


include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
#@mkdir -p $(UNIT_TEST_PATH)/
#@cp -dpRf $< $(UNIT_TEST_PATH)/
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))

