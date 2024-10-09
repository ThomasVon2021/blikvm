#!/bin/bash
set -x

# 检测是否存在 /mnt/tmp/firstboot 文件
check_firstboot() {
  if [ -f "/mnt/tmp/firstboot" ]; then
    echo "Found /mnt/tmp/firstboot file. Resizing mmcblk0p3 partition..."
    resize_mmcblk0p3
    # 删除 /mnt/tmp/firstboot 文件
    rm -f "/mnt/tmp/firstboot"
  else
    echo "No /mnt/tmp/firstboot file found. Skipping partition resizing."
  fi
}

# 扩展 mmcblk0p3 分区

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

# 检测并执行扩容操作
check_firstboot

echo "Partition resizing completed."
cd /mnt/exec/release
/mnt/exec/release/server_app
