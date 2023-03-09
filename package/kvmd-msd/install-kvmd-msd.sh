#!/bin/bash

mkdir -p "/usr/bin/blikvm"
cp kvmd-msd.sh "/usr/bin/blikvm/kvmd-msd.sh"
cp -r ventoy-1.0.88 "/usr/bin/blikvm/"

chmod 777 /usr/bin/blikvm/kvmd-msd.sh

