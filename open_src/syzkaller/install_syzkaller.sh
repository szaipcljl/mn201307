#!/bin/bash
# Generic setup instructions

#
#Install Go 1.8.1:
#
# download Go distribution and unpack Go into a directory
sudo mkdir /go /gopath
wget https://storage.googleapis.com/golang/go1.8.3.linux-amd64.tar.gz
sudo tar -zxvf go1.8.3.linux-amd64.tar.gz -C /go

# set env var
export GOROOT=/go
export PATH=/go/bin:$PATH
export GOPATH=/gopath

#
# Syzkaller
#
# checkout syzkaller sources with all dependencies.
sudo apt install golang-go --allow-unauthenticated
go get -u -d github.com/google/syzkaller/

# build
cd /gopath/src/github.com/google/syzkaller
mkdir workdir
make
