#!/bin/bash
#adduser username会在/home下建立一个文件夹username
adduser $USR

# add sudoers
sudo vi /etc/sudoers
userlist_deny=NO
userlist_enable=YES 
userlist_file=/etc/allowed_users
seccomp_sandbox=NO
local_enable=YES
write_enable=YES
