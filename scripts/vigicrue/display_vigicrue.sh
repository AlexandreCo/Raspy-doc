#!/bin/bash
NB_DAY=$1

LOG_PATH=/var/www/GnOmeDataCenter/log
LOG_OUT=/var/www/GnOmeDataCenter/out
DATE=`date "+%s"`
FILEOUT=$NB_DAY"_vigicrue.png"
export GDFONTPATH=/usr/share/font/

function genere_vigicrue_cmd()
{
cat <<EOF
set xlabel " Hauteurs en m"
set format x "%d/%m"
set autoscale
set ytics 1
set grid
set xdata time
set timefmt "%Y/%m/%d:%H:%M"
set xtics rotate by -60
plot "vigicrue2.dat$DATE"  using 1:3 with lines  title 'Huisne (Pontlieux)'
replot "vigicrue2.dat$DATE"  using 1:2 with lines  title 'Sarthe (Yssoir)'
set term png
set output "$LOG_OUT/$FILEOUT"
replot
EOF
}
function split_file()
{
	for i in `ls $LOG_PATH  | grep "vigicrue" | tail -n $NB_DAY` 
	do
		FILENAME=$i
		cat $LOG_PATH/$FILENAME | awk -F" "  '{print $1"/"$2}' | awk -F"/"  '{print $3"/"$2"/"$1":"$4}'>> vigicrue.dat$DATE
	done
	cat vigicrue.dat$DATE | sort | uniq| grep -v Mans | awk -F"," '{print $1" "$2" "$3}' > vigicrue2.dat$DATE

}

genere_vigicrue_cmd > vigicrue.cmd$DATE
split_file
gnuplot < vigicrue.cmd$DATE
rm vigicrue.cmd$DATE
rm vigicrue.dat$DATE
rm vigicrue2.dat$DATE


