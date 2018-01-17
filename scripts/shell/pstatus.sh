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

_info()
{
	_echo 1 "$@"
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

pstatus red red "red"
pstatus green green "green"
pstatus yellow yellow "yellow"
pstatus blue blue "blue"
