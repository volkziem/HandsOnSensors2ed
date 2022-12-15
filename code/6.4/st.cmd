#!../../bin/linux-arm/temp

## You may have to change temp to something else
## everywhere it appears in this file

< envPaths

epicsEnvSet(STREAM_PROTOCOL_PATH,"../../tempApp/Db")

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/temp.dbd"
temp_registerRecordDeviceDriver pdbbase

drvAsynSerialPortConfigure("SERIALPORT","/dev/ttyACM0",0,0,0)
asynSetOption("SERIALPORT",-1,"baud","9600")
asynSetOption("SERIALPORT",-1,"bits","8")
asynSetOption("SERIALPORT",-1,"parity","none")
asynSetOption("SERIALPORT",-1,"stop","1")
asynSetOption("SERIALPORT",-1,"clocal","Y")
asynSetOption("SERIALPORT",-1,"crtscts","N")

## Load record instances
dbLoadRecords("db/temperature.db","PORT='SERIALPORT',USER='raspi'")

#drvAsynIPPortConfigure("SOCKET","192.168.20.135:1137",0,0,0)
#dbLoadRecords("db/temperature.db","PORT='SOCKET',USER='node'")

cd "${TOP}/iocBoot/${IOC}"
iocInit
