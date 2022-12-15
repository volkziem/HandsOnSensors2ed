% mariadb_read.m, V. Ziemann, 221003
clear all;
sql=mariadb('hostname','localhost','username','me','password','pwpw');
sql.database='readA0';  % select database
request='select * from fdata;';
d=sql.query(request);   % retrieve request
sql.command='quit;';    % close database
d(1,:)=[];              % remove column labels
tt=datenum(d(:,1),'yyyy-mm-dd HH:MM:SS');
d(:,1)=[];              % remove the cells with dates
a=str2double(d);        % convert rest to an array
plot(tt,a(:,1),'k',tt,a(:,2),'r');
datetick('x','ddd/HH:MM');
legend('A0','A1');
