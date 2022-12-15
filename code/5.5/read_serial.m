% read_serial.m
s=serialport("/dev/ttyUSB0",9600);  % open serial line
pause(2)                            % wait for this to complete
reply=queryResponse(s,"A0?\n")      % send query and receive reply
clear s                             % close serial port   

  
