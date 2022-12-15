#!/bin/bash
# fill_weather_db.sh
DB=/home/pi/rrdtool/weather.rrd
TEMP=$(echo "ALL?" | netcat -C 192.168.20.56 1137)
#TEMP=$(/usr/bin/python3 /home/pi/rrdtool/read_from.py 192.168.20.56 T P H VOC)
/usr/bin/rrdtool update $DB N:$TEMP

#echo $TEMP
