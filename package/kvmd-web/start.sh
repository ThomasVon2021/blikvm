#!/bin/bash
set -x

# check /mnt/tmp/firstboot 
check_firstboot() {
  if [ -f "/mnt/tmp/firstboot" ]; then
    echo "Found /mnt/tmp/firstboot file. Resizing mmcblk0p3 partition..."
    resize_mmcblk0p3

    rm -rf /etc/machine-id
    rm -rf /var/lib/dbus/machine-id

    dbus-uuidgen --ensure=/etc/machine-id
    dbus-uuidgen --ensure
    
    rm -f /home/blikvm/.bash_history
    rm -f /root/.bash_history

    # delete /mnt/tmp/firstboot 
    rm -f "/mnt/tmp/firstboot"
  else
    echo "No /mnt/tmp/firstboot file found. Skipping partition resizing."
  fi
}

# expand mmcblk0p3 

resize_mmcblk0p3() {
  echo "Resizing mmcblk0p3 partition..."
  umount /dev/mmcblk0p3
  parted -s /dev/mmcblk0 resizepart 3 100%
  e2fsck -fy /dev/mmcblk0p3
  resize2fs /dev/mmcblk0p3
  mount /dev/mmcblk0p3
  echo "mmcblk0p3 partition resized successfully."
}


echo "Starting partition resizing..."

check_firstboot

echo "Partition resizing completed."
cd /mnt/exec/release
/mnt/exec/release/server_app

wait $!
