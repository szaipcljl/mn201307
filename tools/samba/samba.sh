#!/bin/bash

# check root permission
check_root_privileges()
{
	if [ $UID -eq 0 ]; then
		echo "You have root privileges!"
	else
		echo -e "${color_failed}>>> Error: You don't have root privileges!"
		echo -e "Please input \"sudo ./install.sh\"${color_reset}"
		exit
	fi
}

check_root_privileges

#
# (1)to install the Samba Server and its dependencies with apt.
#
apt-get install -y samba samba-common python-glade2 system-config-samba

#
# (2)To configure samba, edit the file /etc/samba/smb.conf.
#
cp -pf /etc/samba/smb.conf /etc/samba/smb.conf.bak
cat /dev/null  > /etc/samba/smb.conf

# Edit the Samba configuration file, modify $USER to your's. share your home dir
echo -n "Enter your username(home directory name which is the subdir of /home) > "
read USERNAME
echo "You entered: $USERNAME"
cat ./smb_cfg.txt | sed -e "s/nma1x/$USERNAME/g" >> /etc/samba/smb.conf

#
# (3)create a directory for share
#

# create a directory for the anonymous share and set the correct permissions.
# no password, not security
#mkdir -p /samba/anonymous
#chmod -R 0775 /samba/anonymous
#chown -R nobody:nogroup /samba/anonymous

# create a directory for the secured share and set the correct permissions.
addgroup smbgrp
useradd guest -G smbgrp
# Enter the password of the user named 'guest', others can use guest to access
# your shared directory
PASS=123
echo -ne "$PASS\n$PASS\n" | smbpasswd -as guest
#(echo newpassword; echo confirmNewPassword) | smbpasswd -s

mkdir -p /samba/secured
chmod -R 0770 /samba/secured
chown root:smbgrp /samba/secured

echo "Enter the password of the user named '$USERNAME'"
smbpasswd -a $USERNAME

#
# (4)restart Samba to apply the new configuration.
#
service smbd restart

#
# (5) display some infomation
#
echo "login name: $USERNAME	password: what you have entered"
echo "login name: guest		password: $PASS"


#========#
# access #
#========#

# (1) access the Ubuntu sharing on ubuntu
# click files and ctrl+l
# smb://10.239.92.71/
# or
# smb://Mserver
#

# (2) The hostname of my Ubuntu server is Mserver,
# so you can access the Ubuntu sharing in Windows by entering "\\Mserver" in the
# windows search field of the menu or use the network browser of the Windows
# file explorer to cnnect to the share.

# (3) mount samba
#sudo mkdir /mnt/mserver
#sudo mount -o username=nma1x,password=123 //10.239.92.71/nma1x /mnt/mserver/
