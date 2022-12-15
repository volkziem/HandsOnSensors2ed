% send query string and return response up to termination character.
function out=serialReadline(dev,term_char)
  if (nargin==1) term_char=10; end  % defaults to LF=0x0A
  i=1; 
  int_array=uint8(1);
  while true                        % loop forever 
    val=read(dev,1);            % and read one byte
    if (val==term_char) break; end  % until term_char appears
    int_array(i)=val;               % stuff byte in output
    i=i+1;
  end
  out=char(int_array);              % convert to characters
