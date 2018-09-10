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
:<<!
mkdir -p /home/samba/anonymous
chmod -R 0775 /home/samba/anonymous
chown -R nobody:nogroup /home/samba/anonymous
!

# create a directory for the secured share and set the correct permissions.
# add a user to smbgrp
addgroup smbgrp
useradd keeno -G smbgrp
# Enter the password of the user named 'keeno', others can use keeno to access
# your shared directory (smbgrp group)
PASS=123
echo -ne "$PASS\n$PASS\n" | smbpasswd -as keeno
#(echo newpassword; echo confirmNewPassword) | smbpasswd -s

mkdir -p /home/samba/secured
#chmod -R 0770 /home/samba/secured
chmod -R g+w /home/samba/secured
chown root:smbgrp /home/samba/secured

#
# add a smb user, need 'sudo'
#
echo "Enter the password of the user named '$USERNAME'"
sudo smbpasswd -a $USERNAME

#
# (4)restart Samba to apply the new configuration.
#
service smbd restart

#
# (5) display some infomation
#
echo "login name: $USERNAME	password: what you have entered"
echo "/samba/secured(smbgrp) login name: till		password: $PASS"


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
