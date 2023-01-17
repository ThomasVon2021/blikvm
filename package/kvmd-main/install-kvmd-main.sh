#!/bin/bash

mkdir -p "/usr/bin"
cp kvmd-main /usr/bin/kvmd-main

mkdir -p "/usr/lib/systemd/system"
cp kvmd-main.service "/usr/lib/systemd/system"

systemctl enable kvmd-main.service
echo "install kvmd main ok"