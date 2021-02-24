#! /bin/sh

#arduino sketches have to be in a dir of the same name...

arduino-cli compile --fqbn arduino:samd:nano_33_iot $1/ --libraries /home/ubuntu/arduinolibs

arduino-cli upload --port /dev/ttyACM0 --fqbn arduino:samd:nano_33_iot $1/

