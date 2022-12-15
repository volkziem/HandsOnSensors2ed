#!/bin/bash
RASPI=192.168.2.100
ssh -X pi@${RASPI} vncserver -geometry 1280x960 &
sleep 10
vncviewer ${RASPI}:5901 -passwd ${HOME}/.vnc/passwd &

