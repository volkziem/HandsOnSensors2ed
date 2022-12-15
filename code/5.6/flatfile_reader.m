% flatfile viewer, V. Ziemann, 161229
d=importdata('/home/pi/A0.dat');
TZ=1; t=719529+(d(:,1)+TZ*3600)/86400.0;
plot(t,d(:,2))
datetick('x','HH:MM:SS')
