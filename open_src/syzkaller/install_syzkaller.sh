#!/bin/bash
# Generic setup instructions

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
