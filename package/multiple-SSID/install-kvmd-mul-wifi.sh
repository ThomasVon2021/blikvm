#!/bin/bash
if [ -d "/usr/bin/blikvm" ]
then
    echo '/usr/bin/blikvm exists'
else
    mkdir -p "/usr/bin/blikvm"
fi

cp mul-wifi.py /usr/bin/blikvm/mul-wifi.py
cp mul-wifi.sh /usr/bin/blikvm/mul-wifi.sh
cp wifi-config.json /usr/bin/blikvm/wifi-config.json
chmod 777 /usr/bin/blikvm/mul-wifi.py
chmod 777 /usr/bin/blikvm/mul-wifi.sh

mkdir -p "/usr/lib/systemd/system"
cp kvmd-mul-wifi.service "/usr/lib/systemd/system"
systemctl enable kvmd-mul-wifi.service
echo "install mul-wifi ok"