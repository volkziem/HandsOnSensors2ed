% temperature logger, V. Ziemann, 220930
clear all
s=tcpclient("192.168.20.184",1137);
pause(0.01)
running=0;
while running<1000
  running=running+1;
  reply=queryResponse(s,"T?\n");
  val(running)=str2double(reply(2:end));
  x(running)=now;
  plot(x,val,'*')
  ylim([22,28])
  datetick('x','ddd/HH:MM:SS')
  ylabel('Temperature [C]')
  xlabel('Time')
  pause(1);
end
write(s,"quit\n"); 
clear s
