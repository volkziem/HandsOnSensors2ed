#!/bin/bash
DB=/home/pi/rrdtool/db1.rrd
/usr/bin/rrdtool graph /home/pi/public_html/db1.png -s -4h \
	-t "Temperature in my office" -v "T [C]" -l 16 -u 26 -r \
	DEF:t0=$DB:temp:AVERAGE \
	LINE1:t0#FF0000:"Temperature"; 
/usr/bin/rrdtool graph /home/pi/public_html/db2.png -s -2d \
	-t "Temperature in my office" -v "T [C]" -l 16 -u 26 -r \
	DEF:t0=$DB:temp:AVERAGE \
	LINE1:t0#FF0000:"Temperature";
