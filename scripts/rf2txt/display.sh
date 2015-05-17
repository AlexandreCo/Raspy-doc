#!/bin/bash
NB_DAY=$1
ADDRESS=$2
NUM_CPT=$3
COLOR=$4
LOG_PATH=/var/www/GnOmeDataCenter/log_brut
LOG_OUT=/var/www/GnOmeDataCenter/out
DATE=`date "+%s"`
FILEOUT=$ADDRESS"_"$NB_DAY"_"$NUM_CPT"_data.txt.png"
export GDFONTPATH=/usr/share/font/
if [[ $NB_DAY == 1 ]]
then
	XFORMAT="%H:%M "
else

	if [[ $NB_DAY == 7 ]]
	then
		XFORMAT="%H:%M-%d/%m    "
	else
		XFORMAT="%d/%m "
	fi
fi

function genere_single_cmd()
{
cat <<EOF
set terminal pngcairo  enhanced font "arial,10" fontscale 1.0 size 500, 350
set output "$LOG_OUT/$FILEOUT"
set format x "$XFORMAT"
set key off
set style fill   solid 1.00 border lt -1
set xdata time
set xtics border in scale 0,0 nomirror rotate by -45  offset character 0, 0, 0 autojustify
set xtics  norangelimit font ",8"
set xtics   ()
set timefmt "%Y-%m-%d:%H:%M:%S"
plot "single.dat$DATE"  using 1:2 with filledcurves below x1 lt rgb '$COLOR' lw 2
EOF
}

function genere_rain_cmd()
{
cat <<EOF
set terminal pngcairo  enhanced font "arial,10" fontscale 1.0 size 500, 350
set output "$LOG_OUT/$FILEOUT"
set format x "$XFORMAT"
set key off
set style fill   solid 1.00 border lt -1
set xdata time
set xtics border in scale 0,0 nomirror rotate by -45  offset character 0, 0, 0 autojustify
set xtics  norangelimit font ",8"
set xtics   ()
set timefmt "%Y-%m-%d"
plot "rain.dat$DATE"  using 1:2 with filledcurves below x1 lt rgb '$COLOR' lw 2
EOF
}


function genere_minmax_cmd()
{
cat <<EOF
set format x "$XFORMAT"
set key off
set style fill   solid 1.00 border lt -1
set xdata time
set xtics border in scale 0,0 nomirror rotate by -45  offset character 0, 0, 0 autojustify
set xtics  norangelimit font ",8"
set xtics   ()
set timefmt "%Y-%m-%d"
plot "minmax.dat$DATE"  using 1:3 with filledcurves above x1 lt rgb '$COLOR' lw 2
replot "minmax.dat$DATE"  using 1:2 with filledcurves below x1 lt rgb 'white' lw 2
set terminal pngcairo  enhanced font "arial,10" fontscale 1.0 size 500, 350
set output "$LOG_OUT/minmax_$FILEOUT"
replot
EOF
}
function split_file()
{
	for i in `ls $LOG_PATH | grep "^201" | tail -n $NB_DAY` 
	do
		FILENAME=$i
		if [[ $NB_DAY == 1 || $NB_DAY == 7 ]]

		then
			cat $LOG_PATH/$FILENAME | grep "^$ADDRESS;"| awk -F";" '{print $2":"$3" "$NUM_CPT}' NUM_CPT=$NUM_CPT>> single.dat$DATE
		else
			min=$(cat $LOG_PATH/$FILENAME | grep "^$ADDRESS;" |cut -d';' -f$NUM_CPT| sort -n | head -1)
			max=$(cat $LOG_PATH/$FILENAME | grep "^$ADDRESS;" |cut -d';' -f$NUM_CPT| sort -n -r | head -1)
			INDEX_DATE=`echo $FILENAME | cut -f1 -d '_'`
			echo $INDEX_DATE $min $max >> minmax.dat$DATE
		fi
	done
}

function rain_file()
{
	for i in `ls $LOG_PATH | grep "^201" | tail -n $NB_DAY` 
	do
		FILENAME=$i
	
		min=$(cat $LOG_PATH/$FILENAME | grep "^$ADDRESS;" |cut -d';' -f$NUM_CPT| sort -n | head -1)
		max=$(cat $LOG_PATH/$FILENAME | grep "^$ADDRESS;" |cut -d';' -f$NUM_CPT| sort -n -r | head -1)
		
		if [ -n "$min" ]
		then 
			total_rain=`expr $max - $min`
		else 
			total_rain=0
		fi
		INDEX_DATE=`echo $FILENAME | cut -f1 -d '_'`
		echo $INDEX_DATE $total_rain >> rain.dat$DATE
	done
}



if [[ $NUM_CPT == 9 ]]
then 
	genere_rain_cmd >> rain.cmd$DATE
	rain_file
	gnuplot < rain.cmd$DATE
	cp rain.dat$DATE /var/www/GnOmeDataCenter/log_brut/$ADDRESS"_"$NB_DAY"_"$NUM_CPT"_rain.dat"
	rm rain.cmd$DATE
	rm rain.dat$DATE
else
	if [[ $NB_DAY == 1 || $NB_DAY == 7 ]]
	then
		genere_single_cmd > single.cmd$DATE
		split_file
		gnuplot < single.cmd$DATE
		cp single.dat$DATE /var/www/GnOmeDataCenter/log_brut/$ADDRESS"_"$NB_DAY"_"$NUM_CPT"_single.dat"
		rm single.dat$DATE
		rm single.cmd$DATE
	else
		genere_minmax_cmd > minmax.cmd$DATE
		split_file
		gnuplot < minmax.cmd$DATE
		cp minmax.dat$DATE /var/www/GnOmeDataCenter/log_brut/$ADDRESS"_"$NB_DAY"_"$NUM_CPT"_minmax.dat"
		rm minmax.cmd$DATE
		rm minmax.dat$DATE
	fi
fi
