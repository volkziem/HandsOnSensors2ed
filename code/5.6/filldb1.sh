#!/bin/bash
DB=/home/pi/rrdtool/db1.rrd
TEMP=$(/usr/bin/python3 /home/pi/rrdtool/readtemp.py)
/usr/bin/rrdtool update $DB N:$TEMP

