#!/bin/bash
proc_name="server_app"
if [ -d "/mnt/exec" ]
then
    echo '/mnt/exec exists'
else
    mkdir -p "/mnt/exec"
fi
chmod 777 -R /mnt/exec
pid=$(pgrep "${proc_name}")
if [ -n "${pid}" ]; then
  echo "Process $proc_name is running, killing it ..."
  kill -9 "${pid}"
fi


cp -R release /mnt/exec


chmod 777 -R /mnt/exec/release

cp release/lib/janus/* /opt/janus/etc/janus
rm -rf /mnt/exec/release/lib/janus/

mkdir -p "/usr/lib/systemd/system"
cp kvmd-web.service "/usr/lib/systemd/system"
cp start.sh /usr/bin/blikvm/start.sh 

systemctl enable kvmd-web.service

echo "install web ok"