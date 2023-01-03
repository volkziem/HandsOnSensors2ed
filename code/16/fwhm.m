%
% fwhm.m
%
function fwhm=fwhm(data)
N=length(data);
xmax = -1e30;; 
xmin=min(data);
imax=-1;
for i=1:N
    if (data(i) > xmax) 
        xmax=data(i); 
        imax=i;
    end
end
ileft=imax;
while (data(ileft) > (xmax+xmin)/2 && ileft>1)
    ileft=ileft-1;
end
iright=imax;
while (data(iright) > (xmax+xmin)/2 && iright<N-1)
    iright=iright+1;
end
fwhm=iright-ileft-1;