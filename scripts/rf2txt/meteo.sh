#!/bin/sh
# mise a jour toutes les jour
#script nb_jour num_capteur code
# num capteur : 
# pluie    : 198
# Temp ext : 111
# humiditée: 21
# pression : 21
#code :
#  Température 1 : 4	
#  Pression      : 5	
#  Humidité	     : 6
#  Température 2 : 7	
#  Batterie	     : 8
#  Rain          : 9

#temperature
echo "temperature exterieur"
/home/pi/Documents/scripts/rf2txt/display.sh 7 111 4 red > /dev/null
/home/pi/Documents/scripts/rf2txt/display.sh 30 111 4 red > /dev/null
/home/pi/Documents/scripts/rf2txt/display.sh 365 111 4 red > /dev/null

echo "temperature exterieur (soleil)"
/home/pi/Documents/scripts/rf2txt/display.sh 7 198 4 red > /dev/null
/home/pi/Documents/scripts/rf2txt/display.sh 30 198 4 red > /dev/null
/home/pi/Documents/scripts/rf2txt/display.sh 365 198 4 red > /dev/null

#humiditée
#/home/pi/Documents/scripts/rf2txt/display.sh 1 21 6 blue >  /dev/null
#pression
#/home/pi/Documents/scripts/rf2txt/display.sh 1 21 5 green >  /dev/null
#rain
echo "rain"
/home/pi/Documents/scripts/rf2txt/display.sh 7 198 9 blue  >  /dev/null
/home/pi/Documents/scripts/rf2txt/display.sh 30 198 9 blue  >  /dev/null
/home/pi/Documents/scripts/rf2txt/display.sh 395 198 9 blue  >  /dev/null
