#!/bin/bash


opt_no_color=0
opt_verbose=1

__echo()
{
	opt="$1"
	shift
	_msg="$@"
	if [ "$opt_no_color" = 1 ] ; then
		# strip ANSI color codes
		_msg=$(/bin/echo -e  "$_msg" | sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g")
	fi
	# explicitly call /bin/echo to avoid shell builtins that might not take options
	/bin/echo $opt -e "$_msg"
}

_echo_nol()
{
	if [ $opt_verbose -ge $1 ]; then
		shift
		__echo -n "$@"
	fi
}

_info_nol()
{
	_echo_nol 1 "$@"
}

_echo()
{
	if [ $opt_verbose -ge $1 ]; then
		shift
		__echo '' "$@"
	fi
}

_warn()
{
	_echo 0 "\033[31m${@}\033[0m" >&2
}

_info()
{
	_echo 1 "$@"
}

_debug()
{
	_echo 3 "\033[34m(debug) $@\033[0m"
}

pstatus()
{
	if [ "$opt_no_color" = 1 ]; then
		_info_nol "$2"
	else
		case "$1" in
			red)    col="\033[101m\033[30m";;
			green)  col="\033[102m\033[30m";;
			yellow) col="\033[103m\033[30m";;
			blue)   col="\033[104m\033[30m";;
			*)      col="";;
		esac
		_info_nol "$col $2 \033[0m"
	fi
	[ -n "$3" ] && _info_nol " ($3)"
	_info
}

show_header()
{
	_info "\033[1;34mSpectre and Meltdown mitigation detection tool v$VERSION\033[0m"
	_info
}

show_header

if [ "$(id -u)" -ne 0 ]; then
	_warn "Note that you should launch this script with root privileges to get accurate information."
	_warn "We'll proceed but you might see permission denied errors."
	_warn "To run it as root, you can try the following command: sudo $0"
	_warn
fi

pstatus red red "red"
pstatus green green "green"
pstatus yellow yellow "yellow"
pstatus blue blue "blue"

###################
# SPECTRE VARIANT 2
_info "\033[1;34mCVE-2017-5715 [branch target injection] aka 'Spectre Variant 2'\033[0m"

_info "* Mitigation 1"
_info "*   Hardware (CPU microcode) support for mitigation"
_info_nol "*     The SPEC_CTRL MSR is available: "

if [ ! -e /dev/cpu/0/msr ]; then
	# try to load the module ourselves (and remember it so we can rmmod it afterwards)
	modprobe msr 2>/dev/null && insmod_msr=1
	_debug "attempted to load module msr, insmod_msr=$insmod_msr"
fi
if [ ! -e /dev/cpu/0/msr ]; then
	pstatus yellow UNKNOWN "couldn't read /dev/cpu/0/msr, is msr support enabled in your kernel?"
else
	# the new MSR 'SPEC_CTRL' is at offset 0x48
	# here we use dd, it's the same as using 'rdmsr 0x48' but without needing the rdmsr tool
	# if we get a read error, the MSR is not there
	dd if=/dev/cpu/0/msr of=/dev/null bs=8 count=1 skip=9 2>/dev/null
	if [ $? -eq 0 ]; then
		pstatus green YES
	else
		pstatus red NO
	fi
fi

if [ "$insmod_msr" = 1 ]; then
	# if we used modprobe ourselves, rmmod the module
	rmmod msr 2>/dev/null
	_debug "attempted to unload module msr, ret=$?"
fi


# CPUID test
_info_nol "*     The SPEC_CTRL CPUID feature bit is set: "
if [ ! -e /dev/cpu/0/cpuid ]; then
	# try to load the module ourselves (and remember it so we can rmmod it afterwards)
	modprobe cpuid 2>/dev/null && insmod_cpuid=1
	_debug "attempted to load module cpuid, insmod_cpuid=$insmod_cpuid"
fi
if [ ! -e /dev/cpu/0/cpuid ]; then
	pstatus yellow UNKNOWN "couldn't read /dev/cpu/0/cpuid, is cpuid support enabled in your kernel?"
else
	# from kernel src: { X86_FEATURE_SPEC_CTRL,        CPUID_EDX,26, 0x00000007, 0 },
	if [ "$opt_verbose" -ge 3 ]; then
		dd if=/dev/cpu/0/cpuid bs=16 skip=7 iflag=skip_bytes count=1 >/dev/null 2>/dev/null
		_debug "cpuid: reading leaf7 of cpuid on cpu0, ret=$?"
		_debug "cpuid: leaf7 eax-ebx-ecd-edx: "$(dd if=/dev/cpu/0/cpuid bs=16 skip=7 iflag=skip_bytes count=1 2>/dev/null | od -x -A n)
		_debug "cpuid: leaf7 edx higher-half is: "$(dd if=/dev/cpu/0/cpuid bs=16 skip=7 iflag=skip_bytes count=1 2>/dev/null | dd bs=1 skip=15 count=1 2>/dev/null | od -x -A n)
	fi
	# getting high byte of edx on leaf7 of cpuinfo in decimal
	edx_hb=$(dd if=/dev/cpu/0/cpuid bs=16 skip=7 iflag=skip_bytes count=1 2>/dev/null | dd bs=1 skip=15 count=1 2>/dev/null | od -t u -A n | awk '{print $1}')
	_debug "cpuid: leaf7 edx higher byte: $edx_hb (decimal)"
	edx_bit26=$(( edx_hb & 8 ))
	_debug "cpuid: edx_bit26=$edx_bit26"
	if [ "$edx_bit26" -eq 8 ]; then
		pstatus green YES
	else
		pstatus red NO
	fi
fi
