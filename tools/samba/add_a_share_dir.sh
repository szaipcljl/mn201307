#!/bin/bash


#
# (2)To configure samba, edit the file /etc/samba/smb.conf.
#

# Edit the Samba configuration file, modify $USER to your's.
echo -n "Enter the new name > "
read USERNAME
echo "You entered: $USERNAME"
# smb_cfg_for_a_dir.txt: modify and add to /etc/samba/smb.conf

#
# (3)create a directory for share
#

# create a directory for the secured share and set the correct permissions.
#addgroup smbgrp
useradd keeno -G smbgrp
# Enter the password of the user named 'keeno', others can use keeno to access
# your shared directory (smbgrp group)
PASS=123
echo -ne "$PASS\n$PASS\n" | smbpasswd -as keeno
#(echo newpassword; echo confirmNewPassword) | smbpasswd -s

mkdir -p /home/samba/secured
chmod -R 0770 /home/samba/secured
chown root:smbgrp /home/samba/secured

echo "Enter the password of the user named '$USERNAME'"
smbpasswd -a $USERNAME
