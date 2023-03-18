#!/bin/bash


if [ ! -d  "opt/bin/msd/user" ] 
then
  sudo mddir -p /opt/bin/msd/user/
fi

if [ ! -d  "opt/bin/msd/ventoy" ] 
then
  sudo mddir -p /opt/bin/msd/ventoy/
fi

if [ ! -d  "opt/bin/msd/ventoy" ] 
then
mkdir -p "/usr/bin/blikvm"
fi

cp kvmd-msd.sh "/usr/bin/blikvm/kvmd-msd.sh"
cp -r ventoy-1.0.88 "/usr/bin/blikvm/"

chmod 777 /usr/bin/blikvm/kvmd-msd.sh

