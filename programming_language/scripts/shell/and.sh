#!/bin/bash

val=$((12 & 8))
# xx 1000

if [ "$val" -eq 8 ]; then
	echo $val
fi

