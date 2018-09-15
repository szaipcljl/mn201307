#!/bin/bash

# source this_script

ENV_SHELL=$( env | grep bash | awk -F "[/]" '{print $3}')

#ENV_SHELL="zsh"

if [ "$ENV_SHELL" = "bash" ]
then
	# bash
	echo "bash"
	echo "the default shell is "$ENV_SHELL
else
	# zsh
	echo $0
	echo "zsh"
	echo "the default shell is "$ENV_SHELL
fi

#the array's  first element is the current script's path and name
SCRIPT_NAME=${BASH_SOURCE[0]}
echo $SCRIPT_NAME
