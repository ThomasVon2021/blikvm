#!/bin/bash
proc_name="kvmd-main"
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

cp kvmd-main /usr/bin/blikvm/kvmd-main

mkdir -p "/usr/lib/systemd/system"
cp kvmd-main.service "/usr/lib/systemd/system"

systemctl enable kvmd-main.service
echo "install kvmd main ok"
