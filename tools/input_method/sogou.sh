#!/bin/bash
#
# sogou pinyin install
#

# ubuntu12.04 install method
sudo add-apt-repository ppa:fcitx-team/nightly
sudo apt-get update

sudo apt-get install fcitx

# set keyboard input method system
sudo apt-get install im-switch
im-switch -s fcitx -z default

# 从官网下载sogou安装包，双击安装
# 右键点击右上角键盘图标，点击"Input Method Configuration"->"Addon"
# 勾选Advande-> 点击Configure
