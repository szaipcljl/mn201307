#!/bin/bash
foo1()
{
	for x in "$@"
	do
		echo "f1: $x"
	done
}

foo2()
{
	for x in "$*"
	do
		echo "f2: $x"
	done
}

foo3()
{
	echo "f3: $0"
	echo "f3: $1"
	echo "f3: $2"
	echo "f3: $3"
	echo "f3: $4"
	echo "f3: $5"
	echo "f3: $6"
}

foo1 1 2 "3 4 5" 6 7
echo
foo2 1 2 "3 4 5" 6 7
echo
foo3 a b c d e faaaaa
echo
echo $0 $1 $2
# ./foo.sh a b
