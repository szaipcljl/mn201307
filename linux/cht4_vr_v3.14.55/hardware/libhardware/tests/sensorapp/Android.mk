LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

include external/stlport/libstlport.mk

LOCAL_SRC_FILES:= \
	sensor_fw_test.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder \
	liblog \
	libgui \
	libui \
	libstlport


LOCAL_C_INCLUDES += \
	frameworks/base/nitive/include \
	$(call include-path-for, stlport) \
	$(call include-path-for, stlport)/stl \
	$(call include-path-for, stlport)/using/h/



LOCAL_MODULE:= sensorapp

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
