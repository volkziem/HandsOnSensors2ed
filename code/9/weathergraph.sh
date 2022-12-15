#!/bin/bash
S="-3m"
S=$1
DB=/home/pi/rrdtool/weather.rrd
PICDIR=/home/pi/public_html/weather
/usr/bin/rrdtool graph $PICDIR/temperature${S}.png -s $S \
	-w 800 -h 200 \
	-t "Temperature" -v "T [C]" -l 16 -u 32 -r \
	DEF:t0=$DB:T:AVERAGE LINE1:t0#FF0000:"Temperature";
/usr/bin/rrdtool graph  $PICDIR/pressure${S}.png -s $S \
	-w 800 -h 200 \
	-t "Barometric Pressure" -v "P [mbar]" -A -l 975 -u 1025 -r \
	DEF:t0=$DB:P:AVERAGE LINE1:t0#FF0000:"Pressure, range=[975,1025]";
/usr/bin/rrdtool graph $PICDIR/humidity${S}.png -s $S \
	-w 800 -h 200 \
	-t "Humidity" -v "Humidity [%]" -l 0 -u 100 \
	DEF:t0=$DB:H:AVERAGE LINE1:t0#FF0000:"Humidity";
/usr/bin/rrdtool graph $PICDIR/VOC${S}.png -s $S \
	-w 800 -h 200 \
	-t "Air quality" -v "Air  quality [kOhm]" \
	DEF:t0=$DB:VOC:AVERAGE LINE1:t0#FF0000:"Air quality";
