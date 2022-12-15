# serial2msyql.py, V Ziemann, 221003
import serial, time,MySQLdb
ser=serial.Serial("/dev/ttyUSB0",9600,timeout=1)
time.sleep(2)     # wait for serial to be ready
ser.write("A0?\n".encode('utf-8'))
reply=ser.readline().decode('utf-8')
val0=float(reply[3:].strip())
ser.write(b"A1?\n")
reply=ser.readline().decode('utf-8')
val1=float(reply[3:].strip())
ser.close()
db=MySQLdb.connect("localhost","me","pwpw","readA0")
cur=db.cursor()
sql="insert into fdata (A0,A1) values (%f,%f);" % (val0,val1)
print(sql)
cur.execute(sql)
db.commit()
db.close()
