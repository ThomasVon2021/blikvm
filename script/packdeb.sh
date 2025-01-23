#!/bin/bash
### Create package dir (when dpkg -b blikvm-xxx will create blikvm-xxx.deb package)

if [ -z "$1" ] || [ -z "$2" ]; then
  echo "Usage: $0 <version> <hardware_type>"
  exit 1
fi

VERSION=$1
HARDWARE_TYPE=$2


# 定义函数检查并删除目录
check_and_remove_dir() {
  if [ -d "$1" ]; then
    echo "Directory $1 exists. Removing..."
    rm -rf "$1"
  fi
}

# Define board type names
pi4b_board="Raspberry Pi 4 Model B"
cm4b_board="Raspberry Pi Compute Module 4"
h313_board="MangoPi Mcore"

prefix=""

if [ "$HARDWARE_TYPE" == "pi" ]; then
  prefix="v1-v2-v3"
elif [ "$HARDWARE_TYPE" == "allwinner" ]; then
  prefix="v4"
else
  echo "Invalid hardware type. Exiting..."
  exit 1
fi

echo "prefix: $prefix"

check_and_remove_dir "blikvm-$prefix"

cp script/package.json /usr/bin/blikvm
cp /mnt/blikvm/package/kvmd-web/start.sh /usr/bin/blikvm
rm -rf /mnt/exec/*
cp -r /mnt/blikvm/web_src/web_server/release /mnt/exec/
cp /mnt/blikvm/package/kvmd-web/kvmd-web.service /lib/systemd/system/
mkdir -p blikvm-$prefix
cd blikvm-$prefix
mkdir -p DEBIAN

cat << EOF > DEBIAN/control
Package: blikvm
Version: $VERSION
Architecture: arm64
Maintainer: info@blicube.com
Depends: libconfig-dev
Description: Installs blikvm-$VERSION-alpha on the BliKVM
EOF

### Create preinst script
cat << 'EOF' > DEBIAN/preinst
#!/bin/bash
# Stop kvmd-web service before installation
systemctl stop kvmd-web || true
EOF

### Create postinst script
cat << 'EOF' > DEBIAN/postinst
#!/bin/bash
# Start kvmd-web service after installation
systemctl daemon-reload
systemctl enable kvmd-web || true
systemctl start kvmd-web || true
EOF

### Ensure the scripts are executable
chmod 755 DEBIAN/preinst DEBIAN/postinst

### add the files you want to add to the package
tar --exclude='/mnt/exec/release/config/*' -cf - /mnt/exec/ | tar xf -
tar cf - /usr/bin/blikvm | tar xf -
tar cf - /lib/systemd/system/kvmd* | tar xf -

if [ "$prefix" == "v1-v2-v3" ]; then
  tar -cf - /opt/janus/ | tar -xf -
fi

### create deb package using the contents of blikvm-$VERSION directory
cd ..
dpkg -b blikvm-$prefix

# show contents of the deb package and make sure this is what you want
dpkg -c blikvm-$prefix.deb

# when ready, copy the blikvm-$VERSION.deb package to new blikvm v4 package, and run `dpkg -i blikvm-$VERSION.deb` to install it

# verify that blikvm works properly by enabling/starting kvmd-web.service
#systemctl enable --now kvmd-web