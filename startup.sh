#!/usr/bin/bash

#	This script is designed to simplify the dumping of an Arduino project
#	from the Linux command line.
#
#	Exit codes:
#	1 - 9		error when specifying coommand line arguments
#	10 - 19		compilation errors
#	20 - 29		uploading errors

#TODO: make this script more interactive, such as the ability to specify the
#      sketch upload port to as a command line argument

# You can create your settings.sh file with variables and functions:
#	compiler_extra_flags=""		# your compiler flags, such as include paths "-I./include"

upload_args=""		# arguments for the upload_firmware() func
compile_args=""		# arguments for the compile_firmware() func
user_compile_flags=""	# loading from settings.sh

default_firmware_file=./main/main.ino


# $1 - settings file; default 'settings.sh'
function load_settings()
{
	if [ -n "$1" ]
	then
		local settings_file=$1
	else
		local settings_file=./settings.sh
	fi
	if [ ! -f $settings_file ]
	then
		return
	fi

	source $settings_file
	user_compile_flags+=$compiler_extra_flags
}


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
		echo "compilation: I can't find the '$firmware_main_file' file to compile"
		exit 10
	fi


	arduino-cli compile --fqbn arduino:avr:nano \
		--build-property \
			"compiler.cpp.extra_flags=-I./main/include/ -I./main/src/ -I./main/ $user_compile_flags" \
		$firmware_main_file
}


# $1 - device file to upload
# $2 - main firmware file
function upload_firmware()
{
	if [ -n "$1" ]
	then
		local load_device=$1
	else
		local load_device=/dev/ttyUSB0
	fi
	if [ -n "$2" ]
	then
		local firmware_main_file=$2
	else
		local firmware_main_file=$default_firmware_file
	fi


	if [ ! -e $load_device ]
	then
		echo "upload: I can't find a port '$load_device' to upload the sketch to."
		exit 20
	fi
	if [ ! -e $firmware_main_file ]
	then
		echo "upload: I can't find a '$firmware_main_file' firmware file"
		exit 21
	fi


	arduino-cli upload -p $load_device --fqbn arduino:avr:nano $firmware_main_file
}


function print_help_message()
{
	echo "\
This script is designed to automate the process of compiling
and uploading firmware into a microcontroller.

Available command line arguments:

  --help			display this help and exit
  --device-file PATH		path to the file representing your microcontroller
  --firmware-file PATH		path to the main firmware file to pass it in compile func

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
			if [ -n "$2" ]
			then
				upload_args+="$2"
			else
				echo "You need to specify the file that provides the device"
				exit 1
			fi
			shift
			;;
		--firmware-file)
			if [ -n "$2" ]
			then
				compile_args+="$2"
			else
				echo "You need to specify the main firmware file"
				exit 2
			fi
			shift
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
load_settings
compile_firmware $compile_args
upload_firmware $upload_args
