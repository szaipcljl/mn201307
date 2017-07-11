#!/bin/bash

#
# GCC
#
GCC=$HOME/gcc-7.1.0
#sudo apt install subversion --allow-unauthenticated
#svn checkout svn://gcc.gnu.org/svn/gcc/trunk $GCC

cd $HOME
wget http://mirrors-usa.go-parts.com/gcc/releases/gcc-7.1.0/gcc-7.1.0.tar.gz
tar -zxvf gcc-7.1.0.tar.gz
cd $GCC

sudo apt-get install flex bison libc6-dev libc6-dev-i386 linux-libc-dev libgmp3-dev libmpfr-dev libmpc-dev
# linux-libc-dev:i386 

# Build GCC:
mkdir build
mkdir install
cd build/
../configure --enable-languages=c,c++ --disable-bootstrap --enable-checking=no --with-gnu-as --with-gnu-ld --with-ld=/usr/bin/ld.bfd --disable-multilib --prefix=$GCC/install/
make -j($nproc)
make install
