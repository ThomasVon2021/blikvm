#!/bin/bash

mkdir -p "/usr/bin/blikvm"
cp enable-gadget.sh "/usr/bin/blikvm/enable-gadget.sh"
cp disable-gadget.sh "/usr/bin/blikvm/disable-gadget.sh"
chmod 777 /usr/bin/blikvm/enable-gadget.sh
chmod 777 /usr/bin/blikvm/disable-gadget.sh

mkdir -p "/usr/lib/systemd/system"
cp kvmd-hid.service "/usr/lib/systemd/system"

systemctl enable kvmd-hid.service