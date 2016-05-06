#
# Makefile for the input core drivers.
#

# Each configuration option enables a list of files.
obj-$(CONFIG_TOUCHSCREEN_FOCALTECH_V1_3) += fts_ts.o 
fts_ts-y += focaltech_core.o focaltech_ctl.o focaltech_ex_fun.o focaltech_gesture.o focaltech_flash.o 

obj-$(CONFIG_GST_TP_GESTURE_SUPPORT) +=ft_gesture_lib.o
$(obj)/ft_gesture_lib.o: $(srctree)/$(obj)/ft_gesture_lib 
	cp $(srctree)/$(obj)/ft_gesture_lib $(obj)/ft_gesture_lib.o 
#focaltech_ts-y    += focaltech.o focaltech_ctl.o focaltech_ex_fun.o
