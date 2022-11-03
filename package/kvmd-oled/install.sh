#!/bin/bash

mkdir -p "/usr/bin"
cp blikvm-oled.py /usr/bin/blikvm-oled.py
cp kvmd-oled.sh /usr/bin/kvmd-oled.sh
mkdir -p "/usr/lib/systemd/system"
cp kvmd-oled.service "/usr/lib/systemd/system"

echo "install oled ok"