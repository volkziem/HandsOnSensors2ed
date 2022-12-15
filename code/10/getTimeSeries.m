% function out=getTimeSeries(dev,query)
clear all;
s=tcpclient("192.168.20.144",1137);
npts=1024;
write(s,"WF?\n");
pause(1);
data=zeros(1,npts);
for i=1:npts
  data(i)=str2double(tcp_getvalue(s));
end
clear s;
subplot(2,1,1)
plot(data)
xlim([0,npts])
xlabel('Time [ms]')
ylabel('Amplitude [ADC bits]')
subplot(2,1,2);
data=data-mean(data);
fftdata=2*abs(fft(data))/npts;
frequency=(1:npts/2)*500/(npts/2);
plot(frequency,fftdata(1:npts/2))
xlabel('Frequency [Hz]')
ylabel('Spectral density [ADC bits]')
print('spectrum.png','-S1000,700');
