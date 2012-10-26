#!/bin/bash
DATE_START=`date "+%D %X"`
/usr/bin/rsync -az /var/www /media/freebox/Sauvegarde/Rpy/var/
/usr/bin/rsync -az /home/pi/Documents /media/freebox/Sauvegarde/Rpy/home/pi/
DATE_STOP=`date "+%D %X"`
echo "Start=$DATE_START stop=$DATE_STOP" >> /home/pi/Documents/logs/rsync.log
