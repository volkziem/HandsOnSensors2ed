# access MySQL database, V. Ziemann, 221003
import MySQLdb
db=MySQLdb.connect("localhost","me","pwpw","readA0")
cur=db.cursor()
cur.execute("select * from fdata;")
reply=cur.fetchall()
print(reply)
# for r in reply:                           # loop over entries
#   print(r)                             # print each entry
#   print(str(r[0]), str(r[1]), str(r[2])) # format nicely
db.close()
