
#GCC=/home/mlinux/roadefend/rda600_clean/device/s32v/toolchain/gcc-linaro-4.9-2015.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc
GCC=$(TOOLCHAIN_PATH)/aarch64-linux-gnu-gcc

beeper_test: beeper_test.o
	$(GCC) -o beeper_test beeper_test.o
	./cp.sh
beeper_test.o: beeper_test.c
	$(GCC) -c beeper_test.c

clean:
	rm beeper_test beeper_test.o
