#!/bin/sh
#temperature
echo "temperature"
/home/pi/Documents/scripts/rf2txt/display.sh 1 21 7 red
/home/pi/Documents/scripts/rf2txt/display.sh 1 0 4 red

/home/pi/Documents/scripts/rf2txt/display.sh 7 21 7 red
/home/pi/Documents/scripts/rf2txt/display.sh 7 0 4 red

/home/pi/Documents/scripts/rf2txt/display.sh 30 21 7 red
/home/pi/Documents/scripts/rf2txt/display.sh 30 0 4 red

/home/pi/Documents/scripts/rf2txt/display.sh 365 21 7 red
/home/pi/Documents/scripts/rf2txt/display.sh 365 0 4 red

#humiditée
echo "humiditee"
/home/pi/Documents/scripts/rf2txt/display.sh 1 21 6 blue
/home/pi/Documents/scripts/rf2txt/display.sh 7 21 6 blue
/home/pi/Documents/scripts/rf2txt/display.sh 30 21 6 blue
/home/pi/Documents/scripts/rf2txt/display.sh 365 21 6 blue
#pression
echo "pression"
/home/pi/Documents/scripts/rf2txt/display.sh 1 21 5 green
/home/pi/Documents/scripts/rf2txt/display.sh 7 21 5 green
/home/pi/Documents/scripts/rf2txt/display.sh 30 21 5 green
/home/pi/Documents/scripts/rf2txt/display.sh 365 21 5 green
