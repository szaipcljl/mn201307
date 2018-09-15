
#GCC=/home/mlinux/roadefend/rda600_clean/device/s32v/toolchain/gcc-linaro-4.9-2015.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc
GCC=$(TOOLCHAIN_PATH)/aarch64-linux-gnu-gcc
target=dcu_test

$(target): $(target).o
	$(GCC) -o $(target) $(target).o
	./cp.sh
$(target).o: $(target).c
	$(GCC) -c $(target).c

clean:
	rm $(target) $(target).o
