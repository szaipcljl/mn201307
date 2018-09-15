TARGET = $(notdir $(CURDIR))
#CC=$(CROSS_COMPILE)gcc
CC=$(TOOLCHAIN_PATH)/aarch64-linux-gnu-gcc

objs := $(patsubst %c, %o, $(shell ls *.c))

$(TARGET)_test:$(objs)
	$(CC) -o $@ $^
	cp $(TARGET)_test /tftpboot/rfs/home/root

%.o:%.c
	$(CC) -c -o $@ $<
clean:
	rm -f  $(TARGET)_test *.all *.o
