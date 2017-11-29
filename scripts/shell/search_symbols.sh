#!/bin/bash

# search symbols from disassembly file
rm output -rf
mkdir output

#target=./kf4abl.asm
target=./search_symbols_testfile
keywords=("mmc" "emmc" "sd" "flash" "oeminfo" "offset" "index" "kernel" "ramdisk")
direction=("read" "write")

echo "source_keywords: "${source_keywords[*]}
echo "sink_keywords: "${sink_keywords[*]}

# find symbol's defined line, and exchange symbol and address in the format:
# symbol, address
# $1: keyword
# $2: "read" or "write"
search_exchan_and_save()
{
	grep $1 $target | grep $2 | grep ":$" | sed 's/>:$//' | sed 's/<//' |\
		awk 'BEGIN{OFS=FS=":"}{split($a,A,/ /);$a=A[2]" "A[1];print $0}' |\
		sed 's/ /, 0x/g' >> ./output/taint_$2.txt
}

# search seeds and sinks
echo
echo "###### search seeds and sinks #######"

for direct in ${direction[*]}
do
	for keywd in ${keywords[*]}
	do
		echo "=> keyword[$direct]: "$keywd
		search_exchan_and_save $keywd $direct
	done
done

echo
echo "###### remove duplicate in taint_read.txt ######"
awk '!x[$0]++' ./output/taint_read.txt | tee ./output/taint_source.txt
echo
echo "###### remove duplicate in taint_write.txt ######"
awk '!x[$0]++' ./output/taint_write.txt | tee ./output/taint_sink.txt

rm ./output/taint_read.txt ./output/taint_write.txt
