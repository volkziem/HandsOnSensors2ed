# socket_client.py
import socket, atexit, time
def cleanup():
    sock.send(b"quit\n")
    sock.close()
atexit.register(cleanup)    
sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
sock.connect(("192.168.20.184",1137))
sock.send(b"T?\n")
time.sleep(0.1)
reply=sock.recv(1000).decode('utf-8');
print(reply.strip())


 
