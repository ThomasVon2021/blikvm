#!/bin/bash
### Update BliKVM software
### put this file to /usr/local/bin/, then you can use this script in any path.
LOGFILE="/tmp/blikvm-update.log"
/bin/rm -f $LOGFILE

if [ ! -e /opt/bin/blikvm ]; then
  echo "-> BliKVM SW is NOT installed.  Exiting."
  exit 1
else
  echo "-> BliKVM SW is installed.  Proceeding to update SW."
fi

set -x
cd /opt/bin/blikvm/
git stash
git pull
python3 /opt/bin/blikvm/script/update.py | tee -a $LOGFILE
set +x

if [ $( cat $LOGFILE | grep -c 'No update needed' ) -eq 1 ]; then
  echo
  echo "Reboot not required.  Exiting."
else
  echo
  read -p "Press ENTER to reboot for changes to take effect or press CTRL+C to exit." answer
  reboot
fi
