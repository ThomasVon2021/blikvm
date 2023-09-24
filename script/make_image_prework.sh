#!/bin/bash

set -x
# clean history command
history -c
# touch firstboot file
touch /mnt/tmp/firstboot
