#!/bin/bash
# download Go distribution and unpack Go into a directory
mkdir go gocode
wget https://storage.googleapis.com/golang/go1.8.3.linux-amd64.tar.gz
tar -zxvf go1.8.3.linux-amd64.tar.gz go/

# set env var
export GOROOT=$HOME/go
export PATH=$HOME/go/bin:$PATH
export GOPATH=$HOME/gocode

# checkout syzkaller sources with all dependencies.
sudo apt install golang-go -y --force-yes
go get -u -d github.com/google/syzkaller/

