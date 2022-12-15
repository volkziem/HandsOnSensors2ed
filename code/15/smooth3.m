% smooth three consecutive points, V. Ziemann, 170628
function out=smooth3(y);
y=[y(1),y(1:end),y(end)];   % add extremities
f=ones(1,3)/3.0;            % filter function
out=conv(y,f);              % convolute
out=out(3:end-2);           % ensure same length
