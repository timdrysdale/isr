#! /bin/sh

./update.sh encoder

sleep 1 

socat /dev/ttyACM0,echo=0,b57600,crnl -
