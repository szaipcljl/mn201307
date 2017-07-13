#!/bin/bash
# Generic setup instructions

echo "### enter install_syzkaller.sh ###"
# set env var
export GOROOT=$HOME/go
export PATH=$HOME/bin:$PATH
export GOPATH=$HOME/gopath

#
# Syzkaller
#
# checkout syzkaller sources with all dependencies.
go get -u -d github.com/google/syzkaller/...

# build
cd $GOPATH/src/github.com/google/syzkaller
mkdir workdir
make

echo "### end of install_syzkaller.sh ###"
