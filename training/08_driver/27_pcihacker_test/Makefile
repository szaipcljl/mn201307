KERNELDIR ?=/lib/modules/$(shell uname -r)/build

TARGET = pci_hacker
$(TARGET)-objs := pcihacker.o pci_access.o

obj-m += $(TARGET).o

default:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) modules

clean:
	#rm -rf *.ko hello*.o hello*.mod* modules* Mo* .*.cmd .tmp_versions
	make -C $(KERNELDIR) M=`pwd` clean

