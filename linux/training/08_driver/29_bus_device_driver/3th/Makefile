obj-m += device.o device1.o device2.o driver.o driver1.o bus.o

#KDIR := /root/Desktop/drives/nfsroot-29/linux-2.6.29
KDIR ?=/lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=`pwd` modules
clean:
	#make -C $(KDIR) M=`pwd` modules clean
	make -C $(KDIR) M=`pwd` clean
	rm -f modules.order
