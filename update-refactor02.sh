#! /bin/sh

./update.sh refactor02

sleep 1 

socat /dev/ttyACM0,echo=0,b57600,crnl -
