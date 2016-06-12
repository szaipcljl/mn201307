#
# android compilation environment configuration
#

sudo apt-get update
sudo apt-get upgrade -y --force-yes

sudo apt-get install git-core gnupg flex bison gperf libsdl1.2-dev libesd0-dev   libwxgtk2.6-dev build-essential zip curl libncurses5-dev valgrind zlib1g-dev g++-multilib lib32z1-dev lib32ncurses5-dev libxml2-utils

sudo apt-get install gcc-4.5 -y --force-yes
sudo apt-get install gcc-4.5-multilib -y --force-yes
sudo apt-get install gcc-4.5-plugin-dev -y --force-yes
sudo apt-get install gcc-4.5-arm-linux-gnueab -y --force-yes
sudo apt-get install g++-4.5 -y --force-yes
sudo apt-get install g++-4.5-multilib -y --force-yes
sudo apt-get install g++-4.5-arm-linux-gnueabi -y --force-yes
sudo apt-get install g++-4.5-arm-linux-gnueabihf -y --force-yes
cd /usr/bin/
sudo rm g++
sudo rm gcc
sudo ln -s g++-4.5 g++
sudo ln -s gcc-4.5 gcc

sudo apt-get install gitk -y --force-yes
sudo apt-get install qgit -y --force-yes
sudo apt-get install gnome-session-fallback -y --force-yes
sudo apt-get install nautilus-open-terminal -y --force-yes
sudo apt-get install iptux -y --force-yes

# git configuration
git config --global user.name maning
git config --global user.email maning@maning.none
git config --global color.ui true
# Generate git key: id_rsa.pub. Send it to git server manager
ssh-keygen -t rsa
nautilus ~/.ssh/


sudo apt-get install ia32-libs -y --force-yes

###sudo dpkg -i bcompare-3.3.7.15876_amd64.deb

# Android4.4 java : jdk1.6
#cd /usr/local/bin/
#sudo chmod a+x ~/Desktop/jdk-6u45-linux-x64.bin
#sudo cp ~/Desktop/jdk-6u45-linux-x64.bin /usr/local/bin/
#sudo ./jdk-6u45-linux-x64.bin

# Android5.1/6.0 java : jdk1.7
#cd /usr/local/bin
#sudo cp ~/Desktop/jdk-7u65-linux-x64.gz .
#sudo tar -zxvf jdk-7u65-linux-x64.gz
#sudo rm jdk-7u65-linux-x64.gz

#sudo gedit /etc/profile &
