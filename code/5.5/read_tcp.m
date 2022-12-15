s=tcpclient("192.168.20.184",1137);  % open connection
pause(0.1)                           % wait, not really needed 
reply=queryResponse(s,"A0?\n")       % send query and get reply 
write(s,"quit\n");                   % close remote socket
clear s                              % close local socket
