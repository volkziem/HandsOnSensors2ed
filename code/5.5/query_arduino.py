# query_arduino.py, V Ziemann, 220930
import serial, time
query="A0?\n".encode('utf-8')
ser=serial.Serial("/dev/ttyUSB0",9600,timeout=1)
time.sleep(3)     # wait for serial to be ready
ser.write(query)
time.sleep(0.1)
reply=ser.readline().decode('utf-8')
print(reply.strip())
ser.close()
