#!/bin/bash

#
# GCC
#
GCC_VERSION=gcc-7.1.0
GCC_PATH=$HOME/building-gcc-for-android/toolchain/gcc

echo "### enter download_gcc.sh ###"
cd $GCC_PATH

if [ ! -d "$GCC_VERSION" ]; then
	# Control will enter here if $DIRECTORY doesn't exist.
	echo "$GCC_VERSION doesn't exist. Now, we will download $GCC_VERSION.tar.gz"
	wget http://mirrors-usa.go-parts.com/gcc/releases/$GCC_VERSION/$GCC_VERSION.tar.gz
	echo "### Unpack the file with tar! ###"
	tar -zxvf $GCC_VERSION.tar.gz
	rm $GCC_VERSION.tar.gz
else
	echo "$GCC_VERSION exists!"
fi
	echo "path: $GCC_PATH/$GCC_VERSION"
