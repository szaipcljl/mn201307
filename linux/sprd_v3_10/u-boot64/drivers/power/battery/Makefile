#
# Copyright (C) 2012 Samsung Electronics
# Lukasz Majewski <l.majewski@samsung.com>
#
# SPDX-License-Identifier:	GPL-2.0+
#

obj-$(CONFIG_POWER_BATTERY_TRATS) += bat_trats.o
obj-$(CONFIG_POWER_BATTERY_TRATS2) += bat_trats2.o
ifdef CONFIG_ADIE_SC2713S
obj-y += sprd_battery_2713.o
else
obj-y += sprd_battery.o
endif
obj-$(CONFIG_SPRD_EXT_IC_POWER)	+= sprd_chg_helper.o
obj-$(CONFIG_SPRD_EXT_IC_POWER)	+= fan54015.o