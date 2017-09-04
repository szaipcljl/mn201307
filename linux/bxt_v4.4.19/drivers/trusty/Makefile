#
# Makefile for trusty components
#

#KERNELDIR ?=/lib/modules/$(shell uname -r)/build
KERNELDIR ?=$(HOME)/disk2/nma1x-wk/bxt-o/out/target/product/gordon_peak/obj/kernel/lib/modules/4.9.40-quilt-2e5dc0ac-gadd2b4a/build/
#CC := $(HOME)/mn/afl-2.42b/afl-gcc

module:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) modules
	echo $(MAKE)
clean:
	#rm -rf *.ko *.o *.mod* modules* Mo* .*.cmd .tmp_versions
	make -C $(KERNELDIR) M=`pwd` clean

obj-m= \
	trusty.o \
	trusty-irq.o \
	trusty-log.o \
	trusty-mem.o \
	trusty-virtio.o \
	trusty-ipc.o \
	trusty-wall.o \
	trusty-timer.o

