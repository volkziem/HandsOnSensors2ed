# readtemp.py, V Ziemann, 221004
import serial, time
ser=serial.Serial("/dev/ttyUSB0",9600,timeout=1)
time.sleep(2)          # wait for serial to be ready
ser.write(b"T?\n")     # shorthand notation
reply=ser.readline().decode('utf-8')
print(reply[2:].strip())
ser.close()
