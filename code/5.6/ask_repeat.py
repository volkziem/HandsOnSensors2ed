# ask_repeat.py, V. Ziemann, 220930
import serial, time, sys, atexit
def cleanup():
    ser.close()
atexit.register(cleanup)
query="A0?\n".encode('utf-8')
ser=serial.Serial("/dev/ttyUSB0",9600,timeout=1)
time.sleep(3)     # wait for serial to be ready
while 1:
    ser.write(query)
    time.sleep(0.1)
    reply=ser.readline().decode('utf-8')
    print(int(time.time()), reply[3:].strip())
    sys.stdout.flush()
    time.sleep(1)
