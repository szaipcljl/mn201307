#!/system/bin/sh

fw_orig="/system/etc/sensorhub.bin"
fw_cp="/data/sensorhub.bin"
fw_start="0x8000000"
fw_len="0x20000"
#calidata="/sdcard/sensor_calibration_AGM.bin"
calidata="/data/sensor_calibration_AGM.bin"
cali_start="0x8020000"
cali_len="0x114"
ret=0

function check_fw_version {
	echo "maybe new fw upgrade, check version" >> /data/sensorhub_log

    if [ ! -f /data/sh_fw_version ];then
		return 0;
	fi

	read line  < /data/sh_fw_version
	echo "last version  $line" >> /data/sensorhub_log
	grep $line $fw_cp
	if [ $? -eq 0 ];then
		echo "grep ret $?" >> /data/sensorhub_log
		return 1
	fi
}

function do_upgrade {
    if [ ! -f $1 ];then
        echo "$1 not exist" >> /data/sensorhub_log
		return 1
    else
        if [ $1 == $fw_cp ];then			
       		stm32flash -a 0x39 -R -i 360,-341,341:-360,-341,341 -w $1 -S $2:$3 /dev/i2c-3
			sleep 1
			echo "new fw version has been stored at /data/sh_fw_version" >> /data/sensorhub_log
			stm32flash -a 0x39 -R -i 360,-341,341:-360,-341,341 -r /data/sh_fw_version -S 0x8019000:0x9 /dev/i2c-3
		else
	        stm32flash -a 0x39 -R -i 360,-341,341:-360,-341,341 -w $1 -S $2:$3 /dev/i2c-3
		fi

        if [ $? -eq 0 ];then
            echo "upgrade $1 success" >> /data/sensorhub_log
			rm $1
			
			return 0
        else
            echo "$1 upgrade failed, retry for 5 times" >> /data/sensorhub_log
			return 1
        fi
    fi
}

if [ ! -f "/data/sensorhub_log" ];then
	if [ -f $fw_orig ];then
		cp $fw_orig /data/
		sleep 1
	fi
fi

echo "start upgrade" > /data/sensorhub_log

echo "try to upgrade $calidata" >> /data/sensorhub_log
do_upgrade $calidata $cali_start $cali_len
let ret=$ret+$?
echo "ret $ret"

	echo "store cali data" >> /data/sensorhub_log
	sleep 1
	stm32flash -a 0x39 -R -i 360,-341,341:-360,-341,341 -r	/data/sensor_calibrate_status -S $cali_start:$cali_len /dev/i2c-3

echo "try to upgrade $fw_cp" >> /data/sensorhub_log
sleep 1
do_upgrade $fw_cp $fw_start $fw_len
let ret=$ret+$?
echo "ret $ret"

if [ $ret -ne 2 ];then
    echo "trigger reboot" >> /data/sensorhub_log
	sleep 5
	echo "b" > /proc/sysrq-trigger
fi
