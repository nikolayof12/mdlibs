#!/usr/bin/bash

#	This script is designed to simplify the dumping of an Arduino project
#	from the Linux command line.

#TODO: make this script more interactive, such as the ability to specify the
#      sketch upload port to as a command line argument


upload_args=""		# arguments for the upload_firmware() func

default_firmware_file=./main/main.ino


# $1 - main firmware file
function compile_firmware()
{
	if [ -n "$1" ]
	then
		local firmware_main_file=$1
	else
		local firmware_main_file=$default_firmware_file
	fi
	if [ ! -f $firmware_main_file ]
	then
		echo "I can't find the '$firmware_main_file' file to compile"
		exit 10
	fi


	arduino-cli compile --fqbn arduino:avr:nano \
		--build-property \
			"compiler.cpp.extra_flags=-I./main/include/ -I./main/src/ -I./main/" \
		$firmware_main_file
}


# $1 - device file to upload
function upload_firmware()
{
	if [ -n "$1" ]
	then
		local load_device=$1
	else
		local load_device=/dev/ttyUSB0
	fi

	if [ -e $load_device ]
	then
		arduino-cli upload -p $load_device --fqbn arduino:avr:nano ./main/main.ino
	else
		echo
		echo "I can't find a port '$load_device' to upload the sketch to."
	fi
}


function print_help_message()
{
	echo "\
This script is designed to automate the process of compiling
and uploading firmware into a microcontroller.

Available command line arguments:

  --help			display this help and exit
  --device-file PATH		path to the file representing your microcontroller

Examples:
  ./startup.sh					just run if your load device is /dev/ttyUSB0
  ./startup.sh --device-file /dev/ttyUSB3	if your load device is /dev/ttyUSB3 \
"
}


# pass the script execution arguments here
function args_processing()
{
	while [ -n "$1" ]
	do
		case "$1" in
		--help)
			print_help_message
			exit 0
			;;
		--device-file)
			shift
			if [ -n "$1" ]
			then
				upload_args+="$1"
			else
				echo "You need to specify the file that provides the device"
				exit 1
			fi
			;;
		*)
			echo "Option '$1' not found"
			exit 1
			;;
		esac

		shift
	done
}


args_processing $@
compile_firmware
upload_firmware $upload_args
