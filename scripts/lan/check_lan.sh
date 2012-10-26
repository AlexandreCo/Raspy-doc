#!/bin/sh

# cron script for checking wlan connectivity
IP_FOR_TEST="192.168.0.254"
PING_COUNT=1

PING="/bin/ping"
IFUP="/sbin/ifup"
IFDOWN="/sbin/ifdown --force"

INTERFACE="eth0"

FFLAG="/home/pi/Documents/scripts/lan/stuck.fflg"

# ping test
$PING -c $PING_COUNT $IP_FOR_TEST > /dev/null 2> /dev/null
if [ $? -ge 1 ]
then
    logger "$INTERFACE seems to be down, trying to bring it up..."
        if [ -e $FFLAG ]
        then
                logger "$INTERFACE is still down, REBOOT to recover ..."
                rm -f $FFLAG 2>/dev/null
                sudo reboot
        else
                touch $FFLAG
                logger $(sudo $IFDOWN $INTERFACE)
                sleep 10
                logger $(sudo $IFUP $INTERFACE)
        fi
else
#    logger "$INTERFACE is up"
    rm -f $FFLAG 2>/dev/null
fi
