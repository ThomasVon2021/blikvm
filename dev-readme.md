# base raspberry system
- Raspberry Pi OS Lite
- Release date: 20230201
- System: 64-bit
- Kernel version: 5.15
- Debian version: 11 (bullseye)

# Main
```
sudo -i
mkdir /opt/bin

#install wiringpi
#need to enable I2C on hardware(use raspi-config)
git clone https://github.com/WiringPi/WiringPi.git
./build

#Some distributions do not have /usr/local/lib in the default LD_LIBRARY_PATH. To
#fix this, you need to edit /etc/ld.so.conf and add in a single line:
  /usr/local/lib
#then run the ldconfig command.
  sudo ldconfig

# install Adafruit-SSD1306
sudo apt install -y python3-pip
pip install adafruit-circuitpython-ssd1306


cd /opt/bin
apt install git
git clone https://github.com/ThomasVon2021/blikvm.git
cd blikvm/src
make
```


# janus
```
sudo apt install libmicrohttpd-dev libjansson-dev \
    libssl-dev libsrtp2-dev libsofia-sip-ua-dev libglib2.0-dev \
    libopus-dev libogg-dev libcurl4-openssl-dev liblua5.3-dev \
    libconfig-dev pkg-config libtool automake libnice-dev

sudo apt-get install build-essential libspeex-dev libspeexdsp-dev libpulse-dev

git clone https://libwebsockets.org/repo/libwebsockets
cd libwebsockets
# If you want the stable version of libwebsockets, uncomment the next line
# git checkout v3.2-stable
mkdir build
cd build
# See https://github.com/meetecho/janus-gateway/issues/732 re: LWS_MAX_SMP
# See https://github.com/meetecho/janus-gateway/issues/2476 re: LWS_WITHOUT_EXTENSIONS
cmake -DLWS_MAX_SMP=1 -DLWS_WITHOUT_EXTENSIONS=0 -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_C_FLAGS="-fpic" ..
make && sudo make install

cd /opt/bin

git clone https://github.com/meetecho/janus-gateway.git
cd janus-gateway
sh autogen.sh

./configure --prefix=/opt/janus
make
make install

make configs
```

# ustreamer
```
apt-get install libasound2-dev
```

```
cd /opt/bin
apt install build-essential libevent-dev libjpeg-dev libbsd-dev
git clone --depth=1 https://github.com/pikvm/ustreamer
cd ustreamer

ln -s /opt/janus/include/janus /usr/include/janus
sed \
  --in-place \
  --expression 's|^#include "refcount.h"$|#include "../refcount.h"|g' \
  /usr/include/janus/plugins/plugin.h

make WITH_JANUS=1
```

The WITH\_JANUS option compiles the µStreamer Janus plugin and outputs it to janus/libjanus\_ustreamer.so. Move this file to the plugin directory of your Janus installation.

```
mv \
  janus/libjanus_ustreamer.so \
  /opt/janus/lib/janus/plugins/libjanus_ustreamer.so
```

Finally, specify a qualifier for the shared memory object so that the µStreamer Janus plugin can read µStreamer's video data.

```
vim /opt/janus/etc/janus/janus.plugin.ustreamer.jcfg
#add 
memsink: {
  object = "demo::ustreamer::h264"
}
```

# web

```
apt install -y vim libcurl4-openssl-dev
apt install -y  php7.4-bz2 php7.4-cli php7.4-curl php7.4-dev php7.4-json php7.4-mbstring php7.4-xml php7.4-zip php7.4-mysql
```

Optionally, if you would like to change the timezone, edit the `php.ini` file:

```
vim /etc/php/7.4/cli/php.ini
```

Look for `timezone` there and uncomment or add your timezone, for example:
```
date.timezone = Asia/Shanghai
```

Install `pecl` and install `swoole` and `inotify` with it:

```
apt install php-pear

pecl install http://pecl.php.net/get/swoole-4.8.9.tgz
#all is yes
echo "extension=swoole.so" > /etc/php/7.4/cli/conf.d/swoole.ini

pecl install inotify
echo "extension=inotify.so" > /etc/php/7.4/cli/conf.d/inotify.ini
```

## webclient

Install NodeJS first:
```
curl -fsSL https://deb.nodesource.com/setup_18.x | bash - &&\
apt-get install -y nodejs
```

```
npm install
npm run serve
npm run build
```

# Config in raspberry 
in the /boot/config.txt end, add the following content
```
dtoverlay=tc358743
dtoverlay=tc358743-audio
dtoverlay=dwc2
```

# Install kvmd services

```
for service in fan hid main oled web
do
    cd /opt/bin/blikvm/package/kvmd-$service
    bash install.sh
done
```

# Reboot

```
sudo systemctl reboot
```
