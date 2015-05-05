#!/bin/bash
LOG_FILE=/home/pi/Documents/logs/ping
STATE=`cat $LOG_FILE`
if ! ping -c 1 www.google.fr >/dev/null
then 
	echo "ko=$STATE"
	if [ "$STATE" != "ko" ]
	then
		echo "ko" > $LOG_FILE
		wget "http://127.0.0.1/emoncms3/emoncms3/api/post?apikey=be257fcfd02cf344e99ef6a4fcad9c7b&node=666&json={ping:0}"  -qO - >/dev/null
	fi
else
        if [ "$STATE" != "ok" ]
	then    
		echo "ok" > $LOG_FILE
		wget "http://127.0.0.1/emoncms3/emoncms3/api/post?apikey=be257fcfd02cf344e99ef6a4fcad9c7b&node=666&json={ping:1}"  -qO - >/dev/null
	fi      
fi
