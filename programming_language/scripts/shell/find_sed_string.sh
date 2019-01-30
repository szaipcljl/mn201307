#!/bin/bash

#将当前目录下（包括子目录）中所有文件中的RDA602替换为SQ1801
find ./*  -exec  sed -i 's/RDA602/SQ1801/g' {} \;
