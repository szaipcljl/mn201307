#!/bin/bash

# search symbols from disassembly file
#target=./kf4abl.asm
target=./search_symbols_testfile

# search seed
grep "emmc" $target | grep "read" | grep ":$" | sed 's/>:$//' | sed 's/<//' |\
	awk 'BEGIN{OFS=FS=":"}{split($1,A,/ /);$1=A[2]" "A[1];print $0}' |\
	sed 's/ /, 0x/g' | tee taint_emmc_read.txt
grep "mmc" $target | grep "read" | grep ":$" | sed '/emmc/d' | sed 's/>:$//' | sed 's/<//' |\
	awk 'BEGIN{OFS=FS=":"}{split($1,A,/ /);$1=A[2]" "A[1];print $0}' |\
	sed 's/ /, 0x/g' | tee taint_mmc_read.txt

# search sink
grep "emmc" $target | grep "write" | grep ":$" | sed 's/>:$//' | sed 's/<//' |\
	awk 'BEGIN{OFS=FS=":"}{split($1,A,/ /);$1=A[2]" "A[1];print $0}' |\
	sed 's/ /, 0x/g' | tee taint_emmc_write.txt
grep "mmc" $target | grep "write" | grep ":$" | sed '/emmc/d' |\
	sed 's/ /, 0x/g' | sed 's/>:$//' | sed 's/<//' |\
	awk 'BEGIN{OFS=FS=":"}{split($1,A,/ /);$1=A[2]" "A[1];print $0}' |\
	sed 's/ /, 0x/g' | tee taint_mmc_write.txt
grep "flash" $target | grep "write" | grep ":$" | sed 's/>:$//' | sed 's/<//' |\
	awk 'BEGIN{OFS=FS=":"}{split($1,A,/ /);$1=A[2]" "A[1];print $0}' |\
	sed 's/ /, 0x/g' | tee taint_flash_write.txt
