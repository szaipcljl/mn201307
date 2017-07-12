#!/bin/bash

#
#Install Go 1.8.1:
#
# download Go distribution and unpack Go into a directory
cd $HOME
mkdir $HOME/gopath
wget https://storage.googleapis.com/golang/go1.8.3.linux-amd64.tar.gz
tar -zxvf go1.8.3.linux-amd64.tar.gz

sudo apt install golang-go --allow-unauthenticated
