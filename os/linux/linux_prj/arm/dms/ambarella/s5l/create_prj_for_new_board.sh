#!/bin/bash

# after cd to sq1801/

# step 1: replace string
#将当前目录下（包括子目录）中所有文件中的RDA602替换为SQ1801
find ./*  -exec  sed -i 's/RDA602/SQ1801/g' {} \;

# step 2: rename file name
cd config/
rename 's/rda602/sq1801/' *
cd ../bsp/
rename 's/rda602/sq1801/' *
