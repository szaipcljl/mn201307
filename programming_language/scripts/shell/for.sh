#!/bin/bash

echo "(1)-----------------"
N=7
for VARIABLE in 1 2 3 4 5 .. N
do
	echo $VARIABLE
done

echo "(2)-----------------"
file1=./awk.sh
file2=./cut.sh
file3=./date.sh
for VARIABLE in $file1 $file2 $file3
do
	cat $VARIABLE
done

echo "(3)-----------------"
for OUTPUT in $(ls)
do
	echo $OUTPUT
done

echo "(4)-----------------"
for i in {1..5}
do
	echo "Welcome $i times"
done

echo "Bash version ${BASH_VERSION}..."
for i in {0..10..2}
do
	echo "Welcome $i times"
done

echo "(5)-----------------"
#use for as infinite loops
#for (( ; ; ))
#do
#echo "infinite loops [ hit CTRL+C to stop]"
#done
echo "(6)-----------------"
# Conditional exit with break
#for I in 1 2 3 4 5
#do
#statements1      #Executed for all values of ''I'', up to a
#disaster-condition if any.
#statements2
#if (disaster-condition)
#then
#break		#Abandon the loop.
#fi
#statements3          #While good and, no
#disaster-condition.
#done

echo "(7)-----------------"
#Following shell script will go though all files stored in /etc directory. The
#for loop will be abandon when /etc/resolv.conf file found.
for file in /etc/*
do
	if [ "${file}" == "/etc/resolv.conf" ]
	then
		countNameservers=$(grep -c nameserver /etc/resolv.conf)
		echo "Total  ${countNameservers} nameservers defined in ${file}"
		break
	fi
done

echo "(8)-----------------"
#Early continuation with continue statement
#for I in 1 2 3 4 5
#do
	#statements1      #Executed for all values of ''I'', up to a disaster-condition if any.
	#statements2
	#if (condition)
	#then
		#continue   #Go to next iteration of I in the loop and
		#skip statements3
	#fi
	#statements3
#done

echo "(9)-----------------"
#This script make backup of all file names specified on command line. If .bak
#file exists, it will skip the cp command.
FILES="$@"
for f in $FILES
do
	# if .bak backup file exists, read next file
	if [ -f ${f}.bak ]
	then
		echo "Skiping $f file..."
		continue  # read next file and skip
		the cp command
	fi
	# we are here means no
	# backup file exists,
	# just use cp command to
	# copy file
	/bin/cp $f $f.bak
done
