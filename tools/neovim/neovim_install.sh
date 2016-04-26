#!/bin/bash 
#https://github.com/neovim/neovim/wiki/Installing-Neovim
#install it using apt-get on Ubuntu 12.04 and later.


#(1)To be able to use add-apt-repository you may need to install software-properties-common:
sudo apt-get install software-properties-common

#If you're using an older version Ubuntu you have to use:
#sudo apt-get install python-software-properties

#Run the following commands:
sudo add-apt-repository ppa:neovim-ppa/unstable
sudo apt-get update
sudo apt-get install neovim

#(2)Prerequisites for the Python modules:
sudo apt-get install python-dev python-pip python3-dev python3-pip

#If you're using an older version Ubuntu you have to use:
#sudo apt-get install python-dev python-pip python3-dev
#sudo apt-get install python3-setuptools
#sudo easy_install3 pip

#For instructions on how to install the Python modules, see :help nvim_python.

#(3)If you want to use Neovim for some (or all) of the editor alternatives, use the following commands:
#sudo update-alternatives --install /usr/bin/vi vi /usr/bin/nvim 60
#sudo update-alternatives --config vi
#sudo update-alternatives --install /usr/bin/vim vim /usr/bin/nvim 60
#sudo update-alternatives --config vim
#sudo update-alternatives --install /usr/bin/editor editor /usr/bin/nvim 60
#sudo update-alternatives --config editor
