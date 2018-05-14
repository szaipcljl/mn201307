#TARGET = $(notdir $(CURDIR))
TARGET = main

#CC=$(CROSS_COMPILE)gcc
#CC=$(TOOLCHAIN_PATH)/aarch64-linux-gnu-gcc
CC=gcc
CFLAGS=--std=c11

objs := $(patsubst %c, %o, $(shell ls *.c))

$(TARGET):$(objs)
	$(CC) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<
clean:
	rm -f  $(TARGET) *.all *.o
