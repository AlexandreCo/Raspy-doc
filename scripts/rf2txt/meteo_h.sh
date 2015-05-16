#!/bin/sh
# mise a jour toutes les heures
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
/home/pi/Documents/scripts/rf2txt/display.sh 1 198 4 red > /dev/null
/home/pi/Documents/scripts/rf2txt/display.sh 1 111 4 red > /dev/null
#humiditée
#/home/pi/Documents/scripts/rf2txt/display.sh 1 21 6 blue >  /dev/null
#pression
#/home/pi/Documents/scripts/rf2txt/display.sh 1 21 5 green >  /dev/null
#rain
/home/pi/Documents/scripts/rf2txt/display.sh 1 198 9 blue  >  /dev/null
