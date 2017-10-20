#!/bin/bash

# To check if a directory exists in a shell script you can use the following:
if [ -d "./test" ]; then
	echo "Control will enter here if ./test exists."
fi

# Or to check if a directory doesn't exist:
if [ ! -d "./test" ]; then
	echo "Control will enter here if ./test doesn't exist."
fi
