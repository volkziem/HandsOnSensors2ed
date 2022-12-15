% AD5933nwa.m, V. Ziemann, 221202 
clear all; close all;
waiting_factor=1;
s=serialport("/dev/ttyUSB0",115200)  % open serial line
pause(3); flush(s)                     
%configureTerminator(s,"cr/lf","cr/lf")
write(s,"MODE 3\n");
write(s,"PGA 1\n");
write(s,"FREQ 10e3\n");
write(s,"FINC 300\n");
write(s,"NPTS 200\n");
write(s,"STATE?\n"); 
reply=serialReadline(s);
p=sscanf(reply,"STATE %d %d %d %d %d")
%return
%flush(s);
write(s,"SWEEP\n");
pause(2*waiting_factor);
reply="";
while length((r=fread(s))) != 0
  reply=strcat(reply,char(r));
  pause(0.5*waiting_factor);
endwhile
clear s
% reply
[a,count] = sscanf(reply,"%g");
data=reshape(a,[3,count/3])'
sizedata=size(data)
f=(p(1):p(2):p(1)+p(2)*p(3))/1000;
polyabs=polyfit(f',data(:,3),2)
subplot(3,1,1);
plot(f,data(:,3),'k','LineWidth',2,f,polyval(polyabs,f),'r-.','LineWidth',2);
xlabel("frequency [kHz]"); ylabel("abs(Z)")
legend("data","fit"); set(gca,'fontsize',12)
subplot(3,1,2);
phase=atan2(data(:,2),data(:,1))*180/pi;
polyphase=polyfit(f',phase,2)
plot(f,phase,'k','LineWidth',2,f,polyval(polyphase,f),'r-.','LineWidth',2);
xlabel("frequency [kHz]"); ylabel("phase(Z) [deg]")
legend('phase','fit'); set(gca,'fontsize',12)
subplot(3,1,3);
plot(f,data(:,1),'b','LineWidth',2,f,data(:,2),'r-.','LineWidth',2);
xlabel("frequency [kHz]"); ylabel("re(Z),im(Z)")
legend("real(Z)","imag(Z)"); set(gca,'fontsize',12)
Rcal=1;  % calibration resistor
%dlmwrite("calibpoly.dat",[polyabs',polyphase'],"\t")
figure;  %................Impedance and phase
cal=dlmread("calibpoly.dat","\t");
absval=data(:,3);
Zabs=Rcal*polyval(cal(:,1),f)'./absval;
phase=atan2(data(:,2),data(:,1))*180/pi;
Zphase=phase-polyval(cal(:,2),f)';
capacitance_nF=mean(1./(2e6*pi*Zabs.*f'))*1e9  % nF
subplot(2,1,1); plot(f,Zabs,'k','LineWidth',2);
xlabel("frequency [kHz]"); ylabel("abs(Z) [kOhm]")
set(gca,'fontsize',14)
%title(['Capacitance = ',num2str(capacitance_nF,"%4.1f"),' nF'])
subplot(2,1,2); plot(f,Zphase,'r','LineWidth',2);
xlabel("frequency [kHz]"); ylabel("phase(Z) [deg]")
set(gca,'fontsize',14)



