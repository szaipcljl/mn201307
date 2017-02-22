#在android编译环境编译出来的.ko文件可以在手机中insmod
KVERSION := $(ANDROID_PRODUCT_OUT)/obj/KERNEL_OBJ

obj-m := hello.o
#hello-objs := hello-world.o

all:
	make ARCH=arm CROSS_COMPILE=arm-eabi- -C $(KVERSION) M=$(PWD) modules

clean:
	make -C $(KVERSION) M=$(PWD) clean
