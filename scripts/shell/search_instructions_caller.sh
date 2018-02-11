#!/bin/bash

#objdump -d -S vmlinux

keywords_list=(call jmp)
keywords_num=2

# $1: asm file;
# $2: key_word
collect_address()
{
	grep "$2".*\*\% $1 | awk -F ":" '{print $1}' | tee ./tmp/"$2"_sym_addr.txt \
		>>/dev/null
}


# $1: file stored instruction addresses
# $2: kew word;
# $3: Print $2 lines of leading context before matching lines.
# $4: the lines of the file($1)
search_caller()
{
	symbol_addr_file=$1
	prefix=$2
	echo $2"'s caller" >> ./tmp/result/caller_summary.txt

	i=1
	for (( ; ; ))
	do
		addr=$( sed -n $i'p' $symbol_addr_file )

		caller=$( grep $addr -B$3 vmlinux.asm.S 2>/dev/null | \
			grep ">:" 2>/dev/null | \
			tail -n 1 2>/dev/null )

		echo "($i) $addr's caller: $caller"
		echo "($i) $addr's caller: $caller" >> ./tmp/result/caller_summary.txt

		if [ "$i" -ge "$4" ]; then
			break;
		fi
		((i++))
	done
	echo >> ./tmp/result/caller_summary.txt
}

#
search_for_each_keyword()
{

	for keyword in ${keywords_list[*]}
	do
		echo ${keyword}
		collect_address ./vmlinux.asm ${keyword}
		addr_lines=$(wc -l ./tmp/"$keyword"_sym_addr.txt | awk '{print $1}' )
		search_caller ./tmp/"$keyword"_sym_addr.txt ${keyword} "300" "$addr_lines"
	done

}

###########################################################
# step1: prepare
if [ -d ./tmp ]; then
	rm tmp -rf
fi
mkdir tmp
mkdir tmp/result

# search caller for each instruction
search_for_each_keyword
