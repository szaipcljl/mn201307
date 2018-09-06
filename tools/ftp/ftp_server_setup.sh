#!/bin/bash

sudo apt-get install vsftpd
/etc/vsftpd.conf

# stop ，restart ftp service

sudo /etc/init.d/vsftpd start
sudo /etc/init.d/vsftpd stop
sudo /etc/init.d/vsftpd restart


cat /etc/group | grep "ftp"
cat /etc/passwd | grep "ftp"

#ftp服务器的目录位置在 /srv/ftp， 这也是匿名用户访问时的根目录。
#可以使用下列命令来间接更改目录

cd /srv
sudo rm -d ftp
cd ~/
mkdir ftp
sudo ln -s ftp /srv/ftp

#
#配置vsftpd.conf
#

#编辑/etc/vsftpd.conf文件:

#// 允许匿名用户登录
anonymous_enable=YES

#// 允许本地用户登录
local_enable=YES

#// 开启全局上传
write_enable=YES

#// 允许匿名用户上传文件
anon_upload_enable=YES  

#// 充许匿名用户新建文件夹
anon_mkdir_write_enable=YES

# 匿名用户登录

#在/srv/ftp下建立pub用于匿名用户上传的文件，并更改其权限

cd /srv/ftp
sudo mkdir pub
sudo chown ftp:ftp pub

#然后用手机或电脑以匿名用户登录到ftp服务器，在浏览器中 或者
#在文件浏览器的“连接服务器”
#中输入下列地址，即可看到ftp服务器的目录(如果只是想看下，可以先使用匿名用户登录试下)

ftp://localhost
#或
ftp://127.0.0.1
