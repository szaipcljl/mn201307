#
# Copyright 2014 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Sample: This is where we'd set a backup provider if we had one
# $(call inherit-product, device/sample/products/backup_overlay.mk)

# Inherit from the common Open Source product configuration
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.zygote=zygote64_32
PRODUCT_COPY_FILES += system/core/rootdir/init.zygote64_32.rc:root/init.zygote64_32.rc

BOARD_USE_64BIT_USERSPACE := true

# Ring 2 audio policy configuration file
PRODUCT_COPY_FILES += $(LOCAL_PATH)/audio/r2_audio_policy.conf:system/etc/audio_policy.conf

$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, device/intel/cherrytrail/cht_vr/device.mk)

PRODUCT_NAME := r2_cht_vr
PRODUCT_DEVICE := cht_vr
PRODUCT_BRAND := Intel
PRODUCT_MODEL := cht_rvp
PRODUCT_MANUFACTURER := Intel
PRODUCT_LOCALES := en_US en_IN fr_FR it_IT es_ES et_EE de_DE nl_NL cs_CZ pl_PL ja_JP zh_TW zh_CN zh_HK ru_RU ko_KR nb_NO es_US da_DK el_GR tr_TR pt_PT pt_BR rm_CH sv_SE bg_BG ca_ES en_GB fi_FI hi_IN hr_HR hu_HU in_ID iw_IL lt_LT lv_LV ro_RO sk_SK sl_SI sr_RS uk_UA vi_VN tl_PH ar_EG fa_IR th_TH sw_TZ ms_MY af_ZA zu_ZA am_ET hi_IN en_XA ar_XB fr_CA km_KH lo_LA ne_NP mn_MN hy_AM az_AZ ka_GE de_AT de_CH de_LI en_AU en_CA en_NZ en_SG eo_EU fr_CH fr_BE it_CH nl_BE
PRODUCT_AAPT_CONFIG := normal large mdpi
PRODUCT_AAPT_PREF_CONFIG := mdpi

PRODUCT_RESTRICT_VENDOR_FILES := false

$(call inherit-product-if-exists, vendor/intel/cht_vr/device-vendor.mk)
