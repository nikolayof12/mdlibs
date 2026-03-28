#!/usr/bin/bash

#	This script is designed to simplify the dumping of an Arduino project
#	from the Linux command line.

#TODO: make this script more interactive, such as the ability to specify the
#      sketch upload port to as a command line argument


#If your sketch upload port is named differently, change this variable
load_device=/dev/ttyUSB0



function compile_firmware()
{
	arduino-cli compile --fqbn arduino:avr:nano \
		--build-property \
			"compiler.cpp.extra_flags=-I./main/include/ -I./main/src/ -I./main/" \
		./main/main.ino
}


compile_firmware

if [ -e load_device ]
then
	arduino-cli upload -p $load_device --fqbn arduino:avr:nano ./main/main.ino
else
	echo
	echo "I can't find a port '$load_device' to upload the sketch to."
fi
