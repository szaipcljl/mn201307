#!/bin/bash
#
# install wiznote
#
# The binary name is WizNote. Please use WizNote to start program in cmd.

sudo add-apt-repository ppa:wiznote-team
## ubuntu 14.04
#sudo add-apt-repository "deb-src http://ppa.launchpad.net/wiznote-team/ppa/ubuntu trusty main"
sudo apt-get update
sudo apt-get install wiznote

