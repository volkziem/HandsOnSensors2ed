% capacitance_plot.m, V. Ziemann, 221103
% pkg load instrument-control
close all; clear all
s=serialport('/dev/ttyACM0',9600);
pause(2);
flush(s);
write(s,"CAP? 100");
pause(1);
reply=serialReadline(s); 
capacitance=str2double(reply(4:end));
write(s,"TIMESTEP?");
reply=serialReadline(s);
if reply(1:8)=="TIMESTEP"
  timestep=str2double(reply(9:end));
else
  disp(reply)
  clear s;
  return
end
write(s,"WF?");
reply=serialReadline(s);
nsteps=str2num(reply(3:end));
xx=zeros(nsteps,1); 
yy=xx;
for k=1:nsteps
   xx(k)=k*timestep*1e-3;                      % in seconds
   yy(k)=str2double(serialReadline(s))*5/1023; % in Volt
end
clear s
subplot(2,1,1);
plot(xx,yy);             % raw sensor data
xlabel("Time [s]"); ylabel("Voltage [V]");
title(["Capacitance = " num2str(capacitance) " uF"]);
subplot(2,1,2)
semilogy(xx,yy);
xlabel("Time [s]"); ylabel("log(Voltage)"); 
print('capacitance.png','-S1000,700')
