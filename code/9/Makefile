
weather:
        rrdtool create weather.rrd --step 60 \
        DS:T:GAUGE:180:-20:100 \
        DS:P:GAUGE:180:900:1100 \
        DS:H:GAUGE:180:0:100 \
        DS:VOC:GAUGE:180:0:1000 \
        RRA:AVERAGE:0.5:1:2880 \
        RRA:AVERAGE:0.5:10:2880 \
        RRA:AVERAGE:0.5:60:2880

