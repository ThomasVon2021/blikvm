#!/bin/bash

mkdir -p "/usr/bin"
cp kvmd-web.sh /usr/bin/kvmd-web.sh
chmod 777 /usr/bin/kvmd-web.sh

cp kvm-link /usr/bin/kvm-link
chmod 777 /usr/bin/kvm-link

cp config.ini /usr/bin/config.ini
chmod 777 /usr/bin/config.ini

cp admin.ini /usr/bin/admin.ini
chmod 777 /usr/bin/config.ini

mkdir -p "/usr/lib/systemd/system"
cp kvmd-web.service "/usr/lib/systemd/system"

systemctl enable kvmd-web.service

echo "install web ok"