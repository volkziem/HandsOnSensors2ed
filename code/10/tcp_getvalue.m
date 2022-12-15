% tcp_getvalue.m, V. Ziemann, 221202
function out=tcp_getvalue(dev)
i=1;
int_array=uint8(1);
while true
  val=read(dev,1);
  if ((val==',') || (val==0xA)) break; end
  int_array(i)=val;
  i=i+1;
end
out=char(int_array);
