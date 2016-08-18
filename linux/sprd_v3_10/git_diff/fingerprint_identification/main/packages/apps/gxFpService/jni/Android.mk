#
# Copyright 2009 Cedric Priscal
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License. 
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CXXFLAGS := -DHAVE_PTHREADS
LOCAL_CFLAGS := -mfpu=neon -mfloat-abi=softfp -ftree-vectorize -ffast-math -O2
LOCAL_MODULE    :=  libFp
LOCAL_SRC_FILES :=  com_android_server_gxFp_gxFpService.cpp
LOCAL_LDFLAGS += $(LOCAL_PATH)/libfp_client.so
LOCAL_MULTILIB := 32
LOCAL_SHARED_LIBRARIES += \
    libandroid_runtime \
    libandroidfw \
    libbinder \
    libcutils \
    liblog \
    libhardware \
    libhardware_legacy \
    libnativehelper \
    libutils \
    libui \
    libinput \
    libsensorservice \
    libskia \
    libgui \
    libusbhost \
    libsuspend \
    libEGL \
    libGLESv2 \
    libnetutils \

LOCAL_ARM_MODE := arm
LOCAL_LDLIBS    := -lm -llog
include $(BUILD_SHARED_LIBRARY)

