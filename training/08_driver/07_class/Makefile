KERNELDIR ?=/lib/modules/$(shell uname -r)/build
#KERNELDIR ?=$(HOME)/disk2/nma1x-wk/bxt-o/out/target/product/gordon_peak/obj/kernel/lib/modules/4.9.40-quilt-2e5dc0ac-gadd2b4a/build/
# Documentation/kbuild/kbuild.txt

module:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) modules
	rm -rf *.c~
	rm -rf *.mod*
	rm -rf *.o
	rm -rf modules.order
	rm -rf Module.symvers

clean:
	#rm -rf *.ko *.o *.mod* modules* Mo* .*.cmd .tmp_versions
	make -C $(KERNELDIR) M=`pwd` clean

obj-m=class.o
