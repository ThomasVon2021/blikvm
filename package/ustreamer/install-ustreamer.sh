#!/bin/bash
proc_name="ustreamer.bin"
if [ -d "/usr/bin/blikvm" ]
then
    echo '/usr/bin/blikvm exists'
else
    mkdir -p "/usr/bin/blikvm"
fi

pid=$(pgrep "${proc_name}")
if [ -n "${pid}" ]; then
  echo "Process $proc_name is running, killing it ..."
  kill -9 "${pid}"
fi

cp kvmd-video.sh /usr/bin/blikvm/kvmd-video.sh
chmod 777 /usr/bin/blikvm/kvmd-video.sh
cp ustreamer.bin /usr/bin/blikvm/ustreamer.bin
chmod 777 /usr/bin/blikvm/ustreamer.bin
cp edid.txt /usr/bin/blikvm/edid.txt
mkdir -p "/usr/lib/systemd/system"
cp kvmd-video.service "/usr/lib/systemd/system"
systemctl enable kvmd-video.service
echo "install ustreamer ok"