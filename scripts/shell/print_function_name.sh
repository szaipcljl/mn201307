#!/bin/bash

# FUNCNAME is an array

function test_func()
{
	echo "Current $FUNCNAME, \$FUNCNAME => (${FUNCNAME[@]})"
	another_func
	echo "Current $FUNCNAME, \$FUNCNAME => (${FUNCNAME[@]})"
}

function another_func()
{
	echo "Current $FUNCNAME, \$FUNCNAME => (${FUNCNAME[@]})"
}

echo "Out of function, \$FUNCNAME => (${FUNCNAME[@]})"
test_func
echo "Out of function, \$FUNCNAME => (${FUNCNAME[@]})"
