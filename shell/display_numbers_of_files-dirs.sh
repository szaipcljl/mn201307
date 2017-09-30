#!/bin/bash

dir=../shell
if [ -d "$dir" ]; then
	echo "$dir: Number of files is $(find "$dir" -type f | wc -l)"
	echo "$dir: Number of directories is $(find "$dir" -type d | wc -l)"
else
	echo "[ERROR]  Please provide a directory."
	exit 1
fi
