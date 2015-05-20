#!/bin/bash
LOG_PATH=/home/pi/log_rtl433/
logout=/home/pi/log_rtl433/minmax/
DATE=`date "+%s"`

function minmax_day()
{
	nbdays=$1
	deviceid=$2
	sensor=$3
	fileout=$deviceid-$sensor-minmax_days.dat
	rm $logout/$fileout
	for i in `ls $LOG_PATH | grep "^201" | tail -n $nbdays` 
	do
		filename=$i
		min=$(cat $LOG_PATH/$filename | grep "^$deviceid;" |cut -d';' -f$sensor| sort -n | head -1)
		max=$(cat $LOG_PATH/$filename | grep "^$deviceid;" |cut -d';' -f$sensor| sort -n -r | head -1)
		INDEX_DATE=`echo $filename | cut -f1 -d '_'`

		echo "$INDEX_DATE;$min;$max" >> $logout/$fileout
	done
}

minmax_day 365 198 4
minmax_day 365 111 4

