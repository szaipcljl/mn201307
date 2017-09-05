#
# Makefile for trusty components
#

#KERNELDIR ?=/lib/modules/$(shell uname -r)/build
KERNELDIR ?=$(HOME)/disk2/nma1x-wk/bxt-o/out/target/product/gordon_peak/obj/kernel/lib/modules/4.9.40-quilt-2e5dc0ac-gadd2b4a/build/
#CC := $(HOME)/mn/afl-2.42b/afl-gcc
CC := $(HOME)/gcc-6.1.0/install/bin/gcc

# Documentation/kbuild/modules.txt
#ccflags-y := -I$(src)/include
#ccflags-y += -I$(src)/src/hal/include

ccflags-y += -fsanitize=address
# the code coverage options of GCC
ccflags-y += -fprofile-arcs -ftest-coverage

module:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) modules
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

