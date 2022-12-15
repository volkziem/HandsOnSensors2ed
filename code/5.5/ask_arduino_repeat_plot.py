# ask_arduino_repeat_plot.py, V. Ziemann, 220930
import serial, time, atexit
def cleanup():      # ensure serial line is closed after CTRL-c
    ser.close()
atexit.register(cleanup)  # register the cleanup function
query="A0?\n".encode('utf-8')  # the query string
amin=0                         # minimum expected value
amax=5                         # maximum expected value
width=70                       # number of character used for plot
ser=serial.Serial("/dev/ttyUSB0",9600,timeout=1)
lll=len(query)-1    # needed to remove 'A0' from reply
time.sleep(3)       # wait for serial to be ready
t0=time.time()      # starting time
while 1:                       # repeat forever
    ser.write(query)           # send query
    time.sleep(0.1)            # wait a bit
    reply=ser.readline().decode('utf-8')      # read response
    value=reply[lll:].strip()                 # make numeric
    k=int((width-2)*float(value)/(amax-amin)) # where to place *
    p='%8.1f %4s |' % (time.time()-t0,value)
    for j in range(0,width-1):
        if j==k:
            p+='*'
        else:  
            p+=' '
    p+='|'
    print(p)
    time.sleep(1)  # wait a while before next measurement
