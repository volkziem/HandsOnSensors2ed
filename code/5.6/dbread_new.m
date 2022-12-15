% dbread.m
sql=mariadb('hostname','localhost','username','me','password','pwpw');
sql.database='readA0';
%sql.output='mat';
a=sql.query('select * from dat;');
sql.command='quit;';
[tt,aa]=convert_cell_to_array(a)