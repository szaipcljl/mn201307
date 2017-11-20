# This tip might come in handy as well:
# put something like the following into your Makefile:

LISTING = ../bin/kernel.asm
IMAGE = ../bin/kernel.img

ARCH = arm-linux-gnueabi
OBJDUMP = $(ARCH)-objdump

asm:
	$(OBJDUMP) -D -m arm -b binary $(IMAGE) > $(LISTING)

