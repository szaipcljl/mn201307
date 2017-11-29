#!/bin/bash

array_name=(value1 ... valuen)
echo ${array_name[2]}

NAME[0]="Zara"
NAME[1]="Qadir"
NAME[2]="Mahnaz"
NAME[3]="Ayan"
NAME[4]="Daisy"
echo "First Index: ${NAME[0]}"
echo "Second Index: ${NAME[1]}"

#access all the items in an array in one of the following ways:
echo "First Method: ${NAME[*]}"
echo "Second Method: ${NAME[@]}"
