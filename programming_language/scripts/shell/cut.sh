#!/bin/bash
date | cut -d ' ' -f 1
date | awk '{print $1}'

echo "           0" | awk '{print $1}'
echo "           0" | cut -f12 -d ' '
echo "          10" | sed 's/ //g'
