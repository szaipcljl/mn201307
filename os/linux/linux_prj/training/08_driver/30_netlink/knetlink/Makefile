KERNEL_MODULES := netlink

export MODULES_ROOT_DIR := $(shell pwd)
export MODULES_BUILD_DIR := $(MODULES_ROOT_DIR)/build
export KDIR := /lib/modules/$(shell uname -r)/build #这行是为了取出系统下内核的目录(ubuntu)

all: init modules romfs

init:
	mkdir -p $(MODULES_BUILD_DIR)

modules:$(patsubst %, _dir_%, $(KERNEL_MODULES))

$(patsubst %, _dir_%, $(KERNEL_MODULES)):
	@echo
	@echo Building $(patsubst _dir_%, %, $@)
	$(MAKE) -C $(patsubst _dir_%, %, $@) all


romfs: $(patsubst %, _romfs_%, $(KERNEL_MODULES))
$(patsubst %, _romfs_%, $(KERNEL_MODULES)):
	$(MAKE) -C $(patsubst _romfs_%, %, $@) romfs


clean: $(patsubst %, _clean_%, $(KERNEL_MODULES))
	$(RM) $(BUILD_DIR)
$(patsubst %, _clean_%, $(KERNEL_MODULES)):
	@echo
	@echo Cleaning $(patsubst _dir_%, %, $@)
	$(MAKE) -C $(patsubst _clean_%, %, $@) clean

.PHONY:
