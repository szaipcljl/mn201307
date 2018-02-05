#!/bin/bash

app_log_file_name=meltdown.log
dmesg_log_file_name=dmesg.log

format_app_log_file()
{
	meltdown_file=$1
	echo "--- generate $meltdown_file.tmp ---"
	sed -n '/read ff/p' $meltdown_file | \
		sed 's/^[ \t]*//g' | \
		awk '{print $1,$2,$3,$4}' | \
		# delete the line match '= ff'
	sed '/= ff/d' | \
		tee $meltdown_file".tmp" >> /dev/null
}

format_dmesg_file()
{
	dmesg_file=$1
	echo
	echo "--- generate $dmesg_file.tmp ---"
	sed -n '/read ff/p' $dmesg_file | \
		# remove all leading and trailing spaces and tabs from each line in an output.
		sed 's/^[ \t]*//g' | \
		awk '{print $3,$4,$5,$6}' | \
		# remove duplicated line
		awk '!a[$0]++' | \
		sed '/= 00/d' | \
		sed '/= ff/d' | \
		tee $dmesg_file".tmp" >> /dev/null
}


# find duplicated line between two file
search_equal_line_in_two_file()
{
	echo
	echo "--- find duplicated line ---"

	meltdown_file_tmp=$1
	dmesg_file_tmp=$2
	echo $meltdown_file_tmp
	echo $dmesg_file_tmp

	# to count lines in a document. omit the <filename> from the result
	total_lines=$(wc -l < $meltdown_file_tmp)
	echo "total_lines="$total_lines

	echo "--- enter for loop:"
	for (( line=1; ; line++ ))
	do
		if [ $line -gt $total_lines ]; then
			break;
		fi
		pattern=$(sed -n $line'p' $meltdown_file_tmp)
		#((line++))
		echo "for loop: line=$line, pattern='$pattern'"
		# search pattern in file
		sed -n "/$pattern/p" $dmesg_file_tmp >> repeat_line.tmp
	done
	echo "---leave for loop"

	awk '!a[$0]++' repeat_line.tmp | \
		tee repeat_line.out >> /dev/null
	echo "--- result ---"
	cat repeat_line.out
}

######################
# check adb connection
get_file_from_device=0
if [ $get_file_from_device -eq 1 ]; then
	adb root
	if [ $? -ne "0" ]; then
		echo "adb root failed"
		exit
	else
		echo "adb root success"
	fi

	adb push meltdown /data/
	echo "execute meltdown... (maybe > 60s)"
	#adb shell /data/meltdown | tee $app_log_file_name > /dev/null
	#adb shell dmesg | tee $dmesg_log_file_name > /dev/null
fi

########################################
# check duplicated line between two file

cp meltdown.log.bak meltdown.log
cp dmesg.log.bak dmesg.log

format_app_log_file $app_log_file_name
format_dmesg_file $dmesg_log_file_name
search_equal_line_in_two_file $app_log_file_name".tmp" $dmesg_log_file_name".tmp"
#debug file
rm repeat_line.tmp
rm $app_log_file_name".tmp" $dmesg_log_file_name".tmp"
