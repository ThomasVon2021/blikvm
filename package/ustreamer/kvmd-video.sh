#!/bin/bash
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
v4l2-ctl --set-edid=file=/usr/bin/edid.txt --fix-edid-checksums
v4l2-ctl --set-dv-bt-timings query
/usr/bin/ustreamer.bin --device=/dev/video0 --persistent --dv-timings --format=uyvy --encoder=omx --workers=3 --quality=80 --desired-fps=30 --drop-same-frames=30 --last-as-blank=0 --h264-sink=demo::ustreamer::h264 &