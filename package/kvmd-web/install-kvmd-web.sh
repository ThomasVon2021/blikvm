#!/bin/bash
proc_name="server_app"
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

cp -R release /usr/bin/blikvm/
chmod 777 -R /usr/bin/blikvm/release

mkdir -p "/usr/lib/systemd/system"
cp kvmd-web.service "/usr/lib/systemd/system"

systemctl enable kvmd-web.service

echo "install web ok"