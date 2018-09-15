#!/bin/bash

#I found this answer while looking to modify an install script to write an
#install log.

#My script is already full of echo statements like:
# echo "Found OLD run script $oldrunscriptname"
# echo "Please run OLD tmunsetup script first!"
#And I didn't want a tee statement to run it (or another script to call the
#existing one with a tee), so I wrote this:

# A Shell subroutine to echo to screen and a log file

LOGFILE="junklog"

function echolog()
{
	echo $1
	echo $1 >> $LOGFILE
}


echo "Going"
echolog "tojunk"

# eof
#So now in my original script, I can just change 'echo' to 'echolog' where I want
#the output in a log file.
