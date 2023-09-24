#!/bin/bash

cp expand.sh /usr/bin/blikvm/expand.sh

mkdir -p "/usr/lib/systemd/system"
cp expand.service "/usr/lib/systemd/system"

systemctl enable expand.service
echo "install expand ok"