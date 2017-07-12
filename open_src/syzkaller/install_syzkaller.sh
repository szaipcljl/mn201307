#!/bin/bash
# Generic setup instructions

#
#Install Go 1.8.1:
#
# download Go distribution and unpack Go into a directory
cd $HOME
mkdir $HOME/gopath
wget https://storage.googleapis.com/golang/go1.8.3.linux-amd64.tar.gz
tar -zxvf go1.8.3.linux-amd64.tar.gz

# set env var
export GOROOT=$HOME/go
export PATH=$HOME/bin:$PATH
export GOPATH=$HOME/gopath

#
# Syzkaller
#
# checkout syzkaller sources with all dependencies.
sudo apt install golang-go --allow-unauthenticated
go get -u -d github.com/google/syzkaller/...

# build
cd $GOPATH/src/github.com/google/syzkaller
mkdir workdir
make
