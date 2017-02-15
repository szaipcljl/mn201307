#!/bin/bash

# to become the root user
sudo -s

# The first step is to install the Samba Server and its dependencies with apt.
apt-get install -y samba samba-common python-glade2 system-config-samba

# To configure samba, edit the file /etc/samba/smb.conf.
cp -pf /etc/samba/smb.conf /etc/samba/smb.conf.bak
cat /dev/null  > /etc/samba/smb.conf

# Edit the Samba configuration file
cat ./smb_cfg.txt >> /etc/samba/smb.conf

# create a directory for the anonymous share and set the correct permissions.
mkdir -p /samba/anonymous
chmod -R 0775 /samba/anonymous
chown -R nobody:nogroup /samba/anonymous

# create a directory for the secured share and set the correct permissions.
addgroup smbgrp
useradd till -G smbgrp
smbpasswd -a till

mkdir -p /samba/secured
chmod -R 0770 /samba/secured
chown root:smbgrp /samba/secured


# restart Samba to apply the new configuration.
service smbd restart


# you can access the Ubuntu sharing in ubuntu
# click files and ctrl+l
# smb://10.239.92.71/
# or
# smb://Mserver
#

# The hostname of my Ubuntu server is Mserver,
# so you can access the Ubuntu sharing in Windows by entering "\\Mserver" in the
# windows search field of the menu or use the network browser of the Windows
# file explorer to cnnect to the share.

# smbpasswd -a nma1x(username)

# mount samba
sudo mkdir /mnt/mserver
sudo mount -o username=nma1x,password=123 //10.239.92.71/nma1x /mnt/mserver/
