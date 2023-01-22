#!/bin/bash
sleep 10
v4l2-ctl --set-edid=file=/usr/bin/edid.txt --fix-edid-checksums
sleep 1
v4l2-ctl --set-dv-bt-timings query
str1="yes"
while (true)
do
    result=$(ethtool lo | grep "${str1}")
    if [[ "$result" != "" ]]
    then
        echo "network ok"
        break
    else
        sleep 1
        echo "network not ok"
    fi
done

/usr/bin/ustreamer.bin --device=/dev/video0 --persistent --dv-timings --format=uyvy --encoder=omx --workers=3 --quality=80 --desired-fps=30 --drop-same-frames=30 --last-as-blank=0 --h264-sink=demo::ustreamer::h264 &