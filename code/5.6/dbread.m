% dbread.m
mysql('open','localhost','me','pwpw');
mysql('use readA0');
sql='select * from dat;'
%sql='select * from dat where ts > "2017-01-01 17:30" and ts < "2017-01-01 17:55";'
[t,a0,a1]=mysql(sql);
mysql('close');
tt=datenum(t,'yyyy-mm-dd HH:MM:SS');  
plot(tt,a0,tt,a1)
legend('A0','A1')
datetick('x','ddd/HH:MM')

