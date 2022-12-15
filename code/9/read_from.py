# read_from.py reads queries from socket 1137 at IP
import socket, atexit, time, sys
def cleanup():
    sock.send(b"quit\n")
    sock.close()
if len(sys.argv) < 2:
    print("Usage: read_from.py IP <list of queries>")
    sys.exit(2)
else:
    atexit.register(cleanup)
    sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    sock.connect((sys.argv[1],1137)) # socket 1137
    out=''
    for query in sys.argv[2:]:
        bla=query + "?\n"
        sock.send(bla.encode('utf-8'))
        #sock.send(query + "?\n")
        time.sleep(1)
        reply=sock.recv(1000).decode('utf-8');
        #print(reply[len(query):].strip())
        if (len(out))>0:
            out+=':'
        out+=reply[len(query):].strip()
    print(out)
