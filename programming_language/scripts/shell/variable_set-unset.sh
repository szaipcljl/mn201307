#!/bin/bash

# To check for non-null/non-zero string variable, i.e. if set, use
if [ -z "$1" ]; then #This succeeds if $1 is null or unset
	echo "\$1 is unset";
else
	echo "\$1 is set to '$1'";
	var=$1
fi

# To check if a variable is set in Bash?
# where ${var+x} is a parameter expansion which evaluates to nothing if var is
# unset, and substitutes the string x otherwise.
if [ -z ${var+x} ]; then
	echo "var is unset";
else
	echo "var is set to '$var'";
fi
