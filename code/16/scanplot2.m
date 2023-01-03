% scanplot2.m, V. Ziemann, 221103
close all
clear all
xscale=0.161;  % mm/fullstep
s=serialport('/dev/ttyACM0',9600);
pause(3);
flush(s);
nsteps=60;
write(s,"FSCAN 60");
pause(5);
xx=zeros(1,nsteps);
yy=xx;
for i=1:nsteps
   xx(i)=i*xscale;
   yy(i)=str2double(serialReadline(s));
end
clear s
yy=smooth3(yy);          % smooth data
subplot(2,1,1);
plot(xx,yy);             % raw sensor data
%xlabel('x [mm]');
ylabel('arb. units');
subplot(2,1,2);
dy=yy(2:end)-yy(1:end-1);   % derivative
if (yy(1) > yy(end)) dy=-dy; end  
plot(xx(1:end-1),dy);
xlabel('x [mm]');
ylabel('arb. units');
title(['FWHM = ', num2str(xscale*fwhm(dy),"%5.2f"), ' mm'])
print('laser_profile.png','-S1000,700')
