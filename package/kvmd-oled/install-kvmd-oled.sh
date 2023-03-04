#!/bin/bash
if [ -d "/usr/bin/blikvm" ]
then
    echo '/usr/bin/blikvm exists'
else
    mkdir -p "/usr/bin/blikvm"
fi

cp blikvm-oled.py /usr/bin/blikvm/blikvm-oled.py
cp kvmd-oled.sh /usr/bin/blikvm/kvmd-oled.sh
chmod 777 /usr/bin/blikvm/blikvm-oled.py
chmod 777 /usr/bin/blikvm/kvmd-oled.sh

mkdir -p "/usr/lib/systemd/system"
cp kvmd-oled.service "/usr/lib/systemd/system"
systemctl enable kvmd-oled.service
echo "install oled ok"