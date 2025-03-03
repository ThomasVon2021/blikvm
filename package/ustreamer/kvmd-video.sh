#!/bin/bash

# Define board types
pi4b_board="Raspberry Pi 4 Model B"
cm4b_board="Raspberry Pi Compute Module 4"
h616_board="Mango Pi Mcore"

# Define board type values
v2_hat="V3_HAT"
v3_pcie="V2_PCIE"
v4_h616="V4_H616"
unknown="UNKNOWN"

# Function to execute command and get output
exec_cmd() {
  output=$(eval "$1")
  echo "$output"
}

# Function to get board type
get_board_type() {
  # Check if the board is Raspberry Pi 4 Model B
  if [[ $(exec_cmd "cat /proc/cpuinfo") == *"$pi4b_board"* ]] || [[ $(exec_cmd "cat /run/machine.id") == *"$pi4b_board"* ]]; then
    type=$v2_hat
  # Check if the board is Raspberry Pi Compute Module 4
  elif [[ $(exec_cmd "cat /proc/cpuinfo") == *"$cm4b_board"* ]] || [[ $(exec_cmd "cat /run/machine.id") == *"$cm4b_board"* ]]; then
    type=$v3_pcie
  # Check if the board is Mango Pi Mcore
  elif [[ $(exec_cmd "cat /proc/cpuinfo") == *"$h616_board"* ]] || [[ $(exec_cmd "cat /run/machine.id") == *"$h616_board"* ]]; then
    type=$v4_h616
  else
    type=$unknown
  fi
  echo "$type"
}

str1="UP"
while (true)
do
    result=$(ifconfig lo | grep "${str1}")
    if [[ "$result" != "" ]]
    then
        echo "network ok"
        sleep 1
        break
    else
        sleep 1
        echo "network not ok"
    fi
done

sleep 5

# Call the function to get the board type
board_type=$(get_board_type)
echo "Board type: $board_type"

# Perform different actions based on the board type
if [[ "$board_type" == "$v2_hat" ]] || [[ "$board_type" == "$v3_pcie" ]]; then
    v4l2-ctl --set-edid=file=/usr/bin/blikvm/edid.txt --fix-edid-checksums
    v4l2-ctl --set-dv-bt-timings query
    /usr/bin/blikvm/ustreamer.bin --device=/dev/video0 --host=0.0.0.0 --port=8008 --persistent --dv-timings --format=uyvy --encoder=omx --workers=3 --quality=80 --desired-fps=30 --drop-same-frames=30 --last-as-blank=0 --h264-sink=demo::ustreamer::h264
elif [[ "$board_type" == "$v4_h616" ]]; then
  # init device var
  jpeg_supported_device=""
  # ergodic /dev/video device
  for device in /dev/video*; do
    # check support JPEG
    if v4l2-ctl --list-formats-ext -d "$device" | grep -q "JPEG"; then
        jpeg_supported_device="$device"
        break  # find JPEG device
    fi
  done
  if [ -n "$jpeg_supported_device" ]; then
      echo "find support JPEG video divice: $jpeg_supported_device"
      /usr/bin/blikvm/ustreamer.bin --format=MJPEG --device=$jpeg_supported_device --resolution=1920x1080 --host=0.0.0.0 --port=8008 --drop-same-frames=30 --desired-fps=20
  else
      echo "not find JPEG video device, use video1"
      /usr/bin/blikvm/ustreamer.bin --format=MJPEG --device=/dev/video1 --resolution=1920x1080 --host=0.0.0.0 --port=8008 --drop-same-frames=30
  fi
else
  echo "Unknown board type. No action performed."
fi

