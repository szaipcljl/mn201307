#!/bin/bash

#
# Image
#
IMAGE=$HOME/image

# Install debootstrap:
sudo apt-get install debootstrap

# Use create-image.sh to create a minimal Debian-wheezy Linux image. The result should
# be $IMAGE/wheezy.img disk image.
mkdir $IMAGE
cp ./create-image.sh $IMAGE
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
