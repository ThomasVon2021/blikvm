#!/bin/bash

mkdir -p "/usr/lib/systemd/system"
cp kvmd-janus.service "/usr/lib/systemd/system"
systemctl enable kvmd-janus.service
echo "install janus ok"