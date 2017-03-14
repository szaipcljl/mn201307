obj-m = tee_drv.o
SRC=$(src)
LDFLAGS =
KDIR=/lib/modules/$(shell uname -r)/build

EXTRA_CFLAGS += $(BUILD_FLAGS)

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

