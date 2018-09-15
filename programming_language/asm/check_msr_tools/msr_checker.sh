#! /system/bin/sh
# run on android device

msr_val_checker()
{
	/data/check_msr
	msr_is_exist=$?
	# MSR IA32_ARCH_CAPABILITIES test
	#_info_nol "* MSR IA32_ARCH_CAPABILITIES bit0 is set:: "
	echo -n "* MSR IA32_ARCH_CAPABILITIES bit0 is set: "
	if [ "$msr_is_exist" -eq "1" ]; then
		ia32_arch_capabilities_reg=10a
		cpu_no=0
		ia32_arch_capabilities_val=$(peeknpoke s r $cpu_no $ia32_arch_capabilities_reg | cut -d ' ' -f7 | sed 's/0x//g')
		ia32_arch_capabilities_val_bit0=$(( ia32_arch_capabilities_val & 1 ))
		if [ "$ia32_arch_capabilities_val_bit0" -eq 1 ]; then
			#pstatus green YES
			echo "1"
		else
			echo "0"
			#pstatus red NO
		fi
	elif [ "$msr_is_exist" -eq "0" ]; then
		#pstatus yellow UNKNOWN
		echo "msr is not exist: msr_is_exist=$msr_is_exist"
	else
		#pstatus yellow UNKNOWN
		echo "check msr failed: msr_is_exist=$msr_is_exist"
	fi
}

msr_val_checker
