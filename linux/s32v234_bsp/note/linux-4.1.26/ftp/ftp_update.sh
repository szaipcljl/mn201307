#!/bin/bash

HOST='192.168.2.1'
USER='root'
PASSWD=''
FILE='dfss_app'

ftp -n -v $HOST <<EOF
quote USER $USER
quote PASS $PASSWD
binary
put $FILE
quit
EOF

echo -n "md5sum: "
md5sum $FILE
