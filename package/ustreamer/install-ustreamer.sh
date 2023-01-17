#!/bin/bash

mkdir -p "/usr/bin"
cp kvmd-video.sh /usr/bin/kvmd-video.sh
chmod 777 /usr/bin/kvmd-video.sh
cp ustreamer.bin /usr/bin/ustreamer.bin
chmod 777 /usr/bin/ustreamer.bin
cp edid.txt /usr/bin/edid.txt
mkdir -p "/usr/lib/systemd/system"
cp kvmd-video.service "/usr/lib/systemd/system"
systemctl enable kvmd-video.service
echo "install ustreamer ok"