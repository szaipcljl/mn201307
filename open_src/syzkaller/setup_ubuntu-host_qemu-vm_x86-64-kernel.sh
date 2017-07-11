#!/bin/bash
# Setup: Ubuntu host, QEMU vm, x86-64 kernel

#
# GCC
#
GCC=$HOME/gcc-7.1.0
#mkdir $GCC
#sudo apt install subversion --allow-unauthenticated

#svn checkout svn://gcc.gnu.org/svn/gcc/trunk $GCC
cd $HOME
wget http://mirrors-usa.go-parts.com/gcc/releases/gcc-7.1.0/gcc-7.1.0.tar.gz
tar -zxvf gcc-7.1.0.tar.gz
cd $GCC

# to download patch: https://patchwork.ozlabs.org/patch/757421/


sudo apt-get install flex bison libc6-dev libc6-dev-i386 linux-libc-dev libgmp3-dev libmpfr-dev libmpc-dev
# linux-libc-dev:i386 

# Build GCC:

mkdir build
mkdir install
cd build/
../configure --enable-languages=c,c++ --disable-bootstrap --enable-checking=no --with-gnu-as --with-gnu-ld --with-ld=/usr/bin/ld.bfd --disable-multilib --prefix=$GCC/install/
make -j64
make install

#
# kernel
#
KERNEL=$HOME/kernel
mkdir $KERNEL
# Checkout Linux kernel source:
git clone https://github.com/torvalds/linux.git $KERNEL

# Generate default configs:
cd $KERNEL
make defconfig
make kvmconfig

# Now we need to enable some config options required for syzkaller. Edit .config
# file manually and enable:
#CONFIG_KCOV=y
#CONFIG_DEBUG_INFO=y
#CONFIG_KASAN=y
#CONFIG_KASAN_INLINE=y

#=>follow bellow after make oldconfig
#*
#* kmemcheck: trap use of uninitialized memory
#*
#kmemcheck: trap use of uninitialized memory (KMEMCHECK) [N/y/?] n
#KASan: runtime memory debugger (KASAN) [Y/n/?] y
  #Instrumentation type
    #> 1. Outline instrumentation (KASAN_OUTLINE) (NEW)
		#2. Inline instrumentation (KASAN_INLINE) (NEW)
		  #choice[1-2]: 2


# Since enabling these options results in more sub options being available, we
# need to regenerate config. Run this and press enter each time when prompted for
# some config value to leave it as default:

make oldconfig

# Build the kernel with previously built GCC:
make CC='$GCC/install/bin/gcc' -j64

# Now you should have vmlinux (kernel binary) and bzImage (packed kernel image):
# $ ls $KERNEL/vmlinux
# $KERNEL/vmlinux
# $ ls $KERNEL/arch/x86/boot/bzImage 
# $KERNEL/arch/x86/boot/bzImage

#
# Image
#
IMAGE=/image

# Install debootstrap:
sudo apt-get install debootstrap
# Use this script to create a minimal Debian-wheezy Linux image. The result should
# be $IMAGE/wheezy.img disk image.
mkdir $IMAGE
cd $IMAGE
./create-image.sh

# To install other packages (not required to run syzkaller):
sudo chroot wheezy /bin/bash -c "apt-get update; apt-get install -y curl tar time strace gcc make sysbench git vim screen usbutils"

# To install Trinity (not required to run syzkaller):
sudo chroot wheezy /bin/bash -c "mkdir -p ~; cd ~/; wget https://github.com/kernelslacker/trinity/archive/v1.5.tar.gz -O trinity-1.5.tar.gz; tar -xf trinity-1.5.tar.gz"
sudo chroot wheezy /bin/bash -c "cd ~/trinity-1.5 ; ./configure.sh ; make -j16 ; make install"

# To install perf (not required to run syzkaller):
cp -r $KERNEL wheezy/tmp/
sudo chroot wheezy /bin/bash -c "apt-get update; apt-get install -y flex bison python-dev libelf-dev libunwind7-dev libaudit-dev libslang2-dev libperl-dev binutils-dev liblzma-dev libnuma-dev"
sudo chroot wheezy /bin/bash -c "cd /tmp/linux/tools/perf/; make"
sudo chroot wheezy /bin/bash -c "cp /tmp/linux/tools/perf/perf /usr/bin/"
rm -r wheezy/tmp/linux

# 
sudo chroot wheezy /bin/bash -c "apt-get update; apt-get install -y curl tar
time strace gcc make sysbench git vim screen usbutils"
To install Trinity (not required to run syzkaller):

sudo chroot wheezy /bin/bash -c "mkdir -p ~; cd ~/; wget
https://github.com/kernelslacker/trinity/archive/v1.5.tar.gz -O
trinity-1.5.tar.gz; tar -xf trinity-1.5.tar.gz"
sudo chroot wheezy /bin/bash -c "cd ~/trinity-1.5 ; ./configure.sh ; make -j16 ;
make install"
To install perf (not required to run syzkaller):

cp -r $KERNEL wheezy/tmp/
sudo chroot wheezy /bin/bash -c "apt-get update; apt-get install -y flex bison
python-dev libelf-dev libunwind7-dev libaudit-dev libslang2-dev libperl-dev
binutils-dev liblzma-dev libnuma-dev"
sudo chroot wheezy /bin/bash -c "cd /tmp/linux/tools/perf/; make"
sudo chroot wheezy /bin/bash -c "cp /tmp/linux/tools/perf/perf /usr/bin/"
rm -r wheezy/tmp/linux

#
# QEMU
#

# Install QEMU:
sudo apt-get install kvm qemu-kvm

# Make sure the kernel boots and sshd starts:
qemu-system-x86_64 \
	  -kernel $KERNEL/arch/x86/boot/bzImage \
	    -append "console=ttyS0 root=/dev/sda debug earlyprintk=serial
slub_debug=QUZ"\
	  -hda $IMAGE/wheezy.img \
	    -net user,hostfwd=tcp::10021-:22 -net nic \
		  -enable-kvm \
		    -nographic \
			  -m 2G \
			    -smp 2 \
				  -pidfile vm.pid \
				    2>&1 | tee vm.log
#early console in setup code
#early console in extract_kernel
#input_data: 0x0000000005d9e276
#input_len: 0x0000000001da5af3
#output: 0x0000000001000000
#output_len: 0x00000000058799f8
#kernel_total_size: 0x0000000006b63000

#Decompressing Linux... Parsing ELF... done.
#Booting the kernel.
#[    0.000000] Linux version 4.12.0-rc3+ ...
#[    0.000000] Command line: console=ttyS0 root=/dev/sda debug
#earlyprintk=serial
#...
#[ ok ] Starting enhanced syslogd: rsyslogd.
#[ ok ] Starting periodic command scheduler: cron.
#[ ok ] Starting OpenBSD Secure Shell server: sshd.

# After that you should be able to ssh to QEMU instance in another terminal:
ssh -i $IMAGE/ssh/id_rsa -p 10021 -o "StrictHostKeyChecking no" root@localhost

# To kill the running QEMU instance:
kill $(cat vm.pid)

#
# Go
#

#Install Go 1.8.1:
wget https://storage.googleapis.com/golang/go1.8.1.linux-amd64.tar.gz
tar -xf go1.8.1.linux-amd64.tar.gz
mv go goroot
export GOROOT=`pwd`/goroot
export PATH=$PATH:$GOROOT/bin
mkdir gopath
export GOPATH=`pwd`/gopath

#
# syzkaller
#

# Get and build syzkaller:
go get -u -d github.com/google/syzkaller/...
cd gopath/src/github.com/google/syzkaller/
mkdir workdir
make

# Create manager config like this:

{
	"http": "127.0.0.1:56741",
	"workdir": "/gopath/src/github.com/google/syzkaller/workdir",
	"vmlinux": "/linux/upstream/vmlinux",
	"image": "/image/wheezy.img",
	"sshkey": "/image/ssh/id_rsa",
	"syzkaller":
	"/gopath/src/github.com/google/syzkaller",
	"procs": 8,
	"type": "qemu",
	"vm": {
		"count": 4,
		"kernel":
		"/linux/arch/x86/boot/bzImage",
		"cpu": 2,
		"mem":
		2048
	}
}

# Run syzkaller manager:
./bin/syz-manager -config=my.cfg

# Now syzkaller should be running, you can check manager status with your web browser at 127.0.0.1:56741.
