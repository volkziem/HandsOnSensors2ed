

db1:
	rrdtool create db1.rrd --step 60 DS:temp:GAUGE:180:-20:100 RRA:AVERAGE:0.5:1:2880


graph1:
	rrdtool graph db1.png -s -2d -t "Temperature in my office" -v "T [C]" DEF:t0=db1.rrd:temp:AVERAGE LINE1:t0#FF0000:"Temperature";
	display db1.png &


