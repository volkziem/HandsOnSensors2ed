# Epics to MQTT gateway, V. Ziemann, 221027
import socket,atexit,paho.mqtt.client
def cleanup():
    sock.close()
atexit.register(cleanup)

def on_message(c,u,msg):
    print("msg = ",msg.topic," ",msg.payload," ",msg.qos)
#   print("Type = ",type(msg.topic)," ",type(msg.payload))
#   print(type(msg.payload.decode('utf-8')))
    outmsg=msg.topic + " " + msg.payload.decode('utf-8') + "\r\n"
#   print("outmsg = ",outmsg)
    epics.send(outmsg.encode('utf-8'))

mqttc=paho.mqtt.client.Client()
mqttc.connect("localhost",1883)
mqttc.on_message=on_message
mqttc.loop_start()

sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
sock.bind(('',51883))
sock.listen(1)
while 1:
    epics,address = sock.accept()
    print("Connected from ",address)
    while 1:
        msg=epics.recv(1024).decode('utf-8')
        if not msg: break
        words=msg.split()
        if len(words)<2: break;
        if words[0].upper()=="SUBSCRIBE":
             mqttc.subscribe(words[1],1)
             print("Subscribing ", words[0], words[1])
        elif words[0].upper()=="UNSUBSCRIBE":
             mqttc.unsubscribe(words[1])            
             print("unsubscribing ", words[0], words[1])
        else:
            print("Publish:",words[0],words[1])
            mqttc.publish(words[0],words[1])
        pass
    epics.close()
    print("Disconnect from ",address)
    
