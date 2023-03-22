#!/bin/bash


if [ ! -d  "opt/bin/msd/user" ] 
then
  mkdir -p /opt/bin/msd/user/
fi

if [ ! -d  "opt/bin/msd/ventoy" ] 
then
  mkdir -p /opt/bin/msd/ventoy/
fi

if [ ! -d  "/usr/bin/blikvm" ] 
then
  mkdir -p /usr/bin/blikvm
fi

cp kvmd-msd.sh "/usr/bin/blikvm/kvmd-msd.sh"
cp -r ventoy-1.0.88 "/usr/bin/blikvm/"

chmod 755 -R /opt/bin/msd/
chmod 755 /usr/bin/blikvm/kvmd-msd.sh
echo "install msd ok"

