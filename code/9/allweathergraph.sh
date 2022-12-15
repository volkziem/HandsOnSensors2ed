#!/bin/bash
/home/pi/rrdtool/weathergraph.sh -8h > /dev/null
/home/pi/rrdtool/weathergraph.sh -1d > /dev/null
/home/pi/rrdtool/weathergraph.sh -1w > /dev/null
/home/pi/rrdtool/weathergraph.sh -1m > /dev/null
/home/pi/rrdtool/weathergraph.sh -3m > /dev/null

