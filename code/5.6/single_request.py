# single measurement, V Ziemann, 220930
import serial, time
ser=serial.Serial("/dev/ttyUSB0",9600,timeout=1)
time.sleep(2)        # wait for serial to be ready
ser.write(b"A0?\n")  # shorthand notation
time.sleep(0.1)
reply=ser.readline().decode('utf-8')
print(int(time.time()), reply[3:].strip())
ser.close()
