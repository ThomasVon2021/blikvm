# Raspberry Pi system specification under test

- Raspberry Pi OS Lite
- Release date: 2023-11-24
- System: 64-bit
- Kernel version: 6.10
- Debian version: 11 (bullseye)

# Main

```
# for msd script
apt-get install jq

sudo -i
mkdir -p /opt/bin

# install wiringpi
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
git submodule update --init --recursive
cd blikvm/src
make
# For ARM based systems (like the Raspberry Pi), use:
#make RPI=1 SSD1306=1
```

# Janus

Install dependencies:

```
sudo apt install libmicrohttpd-dev libjansson-dev \
    libssl-dev libsrtp2-dev libsofia-sip-ua-dev libglib2.0-dev \
    libopus-dev libogg-dev libcurl4-openssl-dev liblua5.3-dev \
    libconfig-dev pkg-config libtool automake libnice-dev

sudo apt-get install build-essential libspeex-dev libspeexdsp-dev libpulse-dev
```

Clone and install libwebsockets:

```
git clone https://libwebsockets.org/repo/libwebsockets
cd libwebsockets
# If you want the stable version of libwebsockets, uncomment the next line
# git checkout v4.3-stable
mkdir build
cd build
# See https://github.com/meetecho/janus-gateway/issues/732 re: LWS_MAX_SMP
# See https://github.com/meetecho/janus-gateway/issues/2476 re: LWS_WITHOUT_EXTENSIONS
cmake -DLWS_MAX_SMP=1 -DLWS_WITHOUT_EXTENSIONS=0 -DCMAKE_INSTALL_PREFIX:PATH=/usr -DCMAKE_C_FLAGS="-fpic" ..
make && sudo make install
```

Clone and install Janus:

```
cd /opt/bin

git clone https://github.com/meetecho/janus-gateway.git
cd janus-gateway
sh autogen.sh

./configure --prefix=/opt/janus
make
make install

make configs
```

# µStreamer

Additional required dependencies for µStreamer:

```bash
apt-get install -y libasound2-dev
```

Clone and compile µStreamer:

```bash
cd /opt/bin
apt install -y build-essential libevent-dev libjpeg-dev libbsd-dev
git clone --depth=1 https://github.com/pikvm/ustreamer
cd ustreamer

ln -s /opt/janus/include/janus /usr/include/janus
sed \
  --in-place \
  --expression 's|^#include "refcount.h"$|#include "../refcount.h"|g' \
  /usr/include/janus/plugins/plugin.h

make WITH_JANUS=1
```

With the `WITH_JANUS` option you compile the µStreamer Janus plugin and outputs is at: `janus/libjanus_ustreamer.so`. Move this file to the plugin directory of your Janus installation.

```bash
mv \
  janus/libjanus_ustreamer.so \
  /opt/janus/lib/janus/plugins/libjanus_ustreamer.so
```

Finally, create a configuration file for the shared memory object, so that the µStreamer Janus plugin can read µStreamer's video data.

```bash
nano /opt/janus/etc/janus/janus.plugin.ustreamer.jcfg
#add 
memsink: {
  object = "demo::ustreamer::h264"
}
audio: {
    device = "hw:1,0"
    tc358743 = "/dev/video0"
}
```

## web-src

init submodule
```
git submodule update --init --recursive
```
Install NodeJS first:
```bash
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.5/install.sh | bash
source ~/.nvm/nvm.sh
nvm install 18.19.0
```
How to run 
```
// client
cd web_src/web_client
git checkout master
npm install
npm run dev

// server
cd web_src/web_server
git checkout master
npm install
npm run dev
```


# Config overlays in Raspberry Pi

In the` /boot/config.txt` end, add the following content:

```conf
dtoverlay=tc358743
dtoverlay=tc358743-audio
dtoverlay=dwc2
```


# Reboot

```bash
sudo systemctl reboot
```
