#!/bin/bash

#
# GCC
#
#GCC_VERSION=gcc-6.1.0
GCC_VERSION=gcc-7.1.0
GCC=$HOME/$GCC_VERSION

#sudo apt install subversion --allow-unauthenticated
#svn checkout svn://gcc.gnu.org/svn/gcc/trunk $GCC
# svn failed, so wo download gcc source code from the mirrors

echo "### enter build_gcc.sh ###"
cd $HOME
wget http://mirrors-usa.go-parts.com/gcc/releases/$GCC_VERSION/$GCC_VERSION.tar.gz
tar -zxvf $GCC_VERSION.tar.gz

cd $GCC

sudo apt-get install flex bison libc6-dev libc6-dev-i386 linux-libc-dev libgmp3-dev libmpfr-dev libmpc-dev
# linux-libc-dev:i386 

# Build GCC:
mkdir build
mkdir install
cd build/
../configure --enable-languages=c,c++ --disable-bootstrap --enable-checking=no --with-gnu-as --with-gnu-ld --with-ld=/usr/bin/ld.bfd --disable-multilib --prefix=$GCC/install/
make -j$(nproc)
make install

echo "### end of build_gcc.sh ###"
