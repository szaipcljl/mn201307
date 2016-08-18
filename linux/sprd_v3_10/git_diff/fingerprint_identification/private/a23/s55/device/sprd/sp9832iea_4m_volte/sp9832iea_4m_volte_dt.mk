#
# Copyright (C) 2007 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
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

TARGET_PLATFORM := sc9630
PLATDIR := device/sprd/scx35l

TARGET_BOARD := sp9832iea_4m_volte
BOARDDIR := device/sprd/$(TARGET_BOARD)

include $(APPLY_PRODUCT_REVISION)

BOARD_KERNEL_PAGESIZE := 2048
BOARD_KERNEL_SEPARATED_DT := true

STORAGE_INTERNAL := emulated
STORAGE_PRIMARY := internal

VOLTE_SERVICE_ENABLE := true

# copy media_codec.xml before calling device.mk,
# because we want to use our file, not the common one
PRODUCT_COPY_FILES += $(BOARDDIR)/media_codecs.xml:system/etc/media_codecs.xml

# SPRD:resolve the primary card can't be recorgnized {@
ifndef STORAGE_ORIGINAL
  STORAGE_ORIGINAL := false
endif

ifndef ENABLE_OTG_USBDISK
  ENABLE_OTG_USBDISK := false
endif
# @}
# SPRD: add for low-memory.set before calling device.mk @{
PRODUCT_RAM := high
# @}
# include general common configs
$(call inherit-product, vendor/sprd/open-source/res/productinfo/connectivity_9830aec_4m.mk)
$(call inherit-product, $(PLATDIR)/device.mk)
$(call inherit-product, $(PLATDIR)/emmc/emmc_device.mk)
$(call inherit-product, $(PLATDIR)/proprietories.mk)

DEVICE_PACKAGE_OVERLAYS := $(BOARDDIR)/overlay $(PLATDIR)/overlay
# Remove video wallpaper feature
PRODUCT_VIDEO_WALLPAPERS := none
BUILD_FPGA := false
PRODUCT_AAPT_CONFIG := hdpi xhdpi normal
PRODUCT_AAPT_PREF_CONFIG := xhdpi

# Set default USB interface
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
	persist.sys.usb.config=mass_storage

PRODUCT_PROPERTY_OVERRIDES += \
	keyguard.no_require_sim=true \
	ro.com.android.dataroaming=false \
	ro.msms.phone_count=2 \
        ro.modem.l.count=2 \
	persist.msms.phone_count=2 \
        persist.radio.multisim.config=dsds \
	persist.msms.phone_default=0 \
        persist.sys.modem.diag=,gser \
        sys.usb.gser.count=8 \
        ro.modem.external.enable=0 \
        persist.sys.support.vt=true \
        persist.sys.volte.enable=true \
        persist.modem.l.cs=0 \
        persist.modem.l.ps=1 \
        persist.modem.l.rsim=1 \
        persist.radio.ssda.mode=fdd-csfb \
        persist.radio.ssda.testmode=6 \
        persist.radio.ssda.testmode1=10 \
        persist.support.oplpnn=true \
        persist.support.cphsfirst=false \
        lmk.autocalc=false \
        use_brcm_fm_chip=true

ifeq ($(strip $(VOLTE_SERVICE_ENABLE)), true)
PRODUCT_PROPERTY_OVERRIDES += persist.sys.volte.enable=true
endif

# board-specific modules
PRODUCT_PACKAGES += \
        sensors.sc8830 \
        fm.$(TARGET_PLATFORM) \
        ValidationTools \
		download \
		gnss_download

#[[ for autotest
        PRODUCT_PACKAGES += autotest
#]]

PRODUCT_PACKAGES += wpa_supplicant \
	wpa_supplicant.conf \
	wpa_supplicant_overlay.conf \
	hostapd

# board-specific files
PRODUCT_COPY_FILES += \
	$(BOARDDIR)/slog_modem_$(TARGET_BUILD_VARIANT).conf:system/etc/slog_modem.conf \
	$(BOARDDIR)/sprd-gpio-keys.kl:system/usr/keylayout/sprd-gpio-keys.kl \
	$(BOARDDIR)/sprd-eic-keys.kl:system/usr/keylayout/sprd-eic-keys.kl \
	$(BOARDDIR)/init.board.2342.rc:root/init.board.rc \
	$(BOARDDIR)/init.recovery.board.2342.rc:root/init.recovery.board.rc \
	$(BOARDDIR)/audio_params/tiny_hw.xml:system/etc/tiny_hw.xml \
	$(BOARDDIR)/audio_params/codec_pga.xml:system/etc/codec_pga.xml \
	$(BOARDDIR)/audio_params/audio_hw.xml:system/etc/audio_hw.xml \
	$(BOARDDIR)/audio_params/audio_para:system/etc/audio_para \
	$(BOARDDIR)/audio_params/audio_policy.conf:system/etc/audio_policy.conf \
	$(BOARDDIR)/focaltech_ts.idc:system/usr/idc/focaltech_ts.idc \
	$(BOARDDIR)/msg2138_ts.idc:system/usr/idc/msg2138_ts.idc \
	frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
	frameworks/native/data/etc/android.hardware.camera.flash.xml:system/etc/permissions/android.hardware.camera.flash.xml \
	frameworks/native/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
	frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
	frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
	frameworks/native/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
	frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml
#	hardware/broadcom/libbt/conf/bcm/firmware/bcm4343s/bcm4343.hcd:system/vendor/firmware/bcm4343.hcd

$(call inherit-product-if-exists, vendor/sprd/open-source/common_packages.mk)
$(call inherit-product-if-exists, vendor/sprd/open-source/plus_special_packages.mk)
$(call inherit-product, vendor/sprd/partner/shark/bluetooth/device-shark-bt.mk)
$(call inherit-product, vendor/sprd/gps/gnss/device-sprd-gps.mk)


PRODUCT_SP9832IEE_4M_VOLTE := true
WCN_EXTENSION := true
CHIP_SP9832IEA := true

# Overrides
PRODUCT_NAME := sp9832iea_4m_volte_dt
PRODUCT_DEVICE := $(TARGET_BOARD)
PRODUCT_MODEL := SP9830I
PRODUCT_BRAND := SPRD
PRODUCT_MANUFACTURER := SPRD

PRODUCT_LOCALES := zh_CN zh_TW en_US
