# Raspberry Pi system specification under test

- Raspberry Pi OS Lite
- Release date: 20230201
- System: 64-bit
- Kernel version: 5.15
- Debian version: 11 (bullseye)

# Main

```
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

# Prepare web server

<details>
<summary>old php code </summary>
Install dependencies:

```bash
apt install -y libcurl4-openssl-dev
```

Install PHP (see below for PHP v8.2):

```bash
apt install -y  php7.4-bz2 php7.4-cli php7.4-curl php7.4-dev php7.4-json php7.4-mbstring php7.4-xml php7.4-zip php7.4-mysql
```

Or when PHP v8.2 is available on your system use:

```bash
apt install -y php8.2-bz2 php8.2-cli php8.2-curl php8.2-dev php8.2-mbstring php8.2-xml php8.2-zip php8.2-mysql
```

Optionally, if you would like to change the timezone, edit the `php.ini` file:

```bash
nano /etc/php/7.4/cli/php.ini
```

Look for `timezone` there and uncomment or add your timezone, for example:

```ini
date.timezone = Asia/Shanghai
```

Install `pecl` and install `swoole` and `inotify` with it:

```bash
apt install php-pear

pecl install http://pecl.php.net/get/swoole-5.0.3.tgz
#all is yes
echo "extension=swoole.so" > /etc/php/7.4/cli/conf.d/swoole.ini

pecl install inotify
echo "extension=inotify.so" > /etc/php/7.4/cli/conf.d/inotify.ini
```
</details>


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
npm install
npm run dev

// server
cd web_src/web_server
npm install
npm run dev
```
How to build
```
cd web_src
bash pack.sh
//Then you can find the binary web_src/web_server/release
The default executable package path is in the/mnt/exec path, and you can replace its corresponding path with the server_app to complete the replacement.
```


# Config overlays in Raspberry Pi

In the` /boot/config.txt` end, add the following content:

```conf
dtoverlay=tc358743
dtoverlay=tc358743-audio
dtoverlay=dwc2
```

# Install kvmd services

```bash
# Install the main controller service
cd /opt/bin/blikvm/package/kvmd-main
./install-kvmd-main.sh

# Install the ustreamer service
cd /opt/bin/blikvm/package/ustreamer
./install-ustreamer.sh 

# Install the HID service
cd /opt/bin/blikvm/package/kvmd-hid
./install-kvmd-hid.sh

# Install the web service
cd /opt/bin/blikvm/package/kvmd-web
./install-kvmd-web.sh

# Install the MSD service
cd /opt/bin/blikvm/package/kvmd-msd
./install-kvmd-msd.sh

# Install the OLED service
cd /opt/bin/blikvm/package/kvmd-oled
./install-kvmd-oled.sh

# Install the fan service
cd /opt/bin/blikvm/package/kvmd-fan
./install-kvmd-fan.sh

# Copy the package.json file
cp /opt/bin/blikvm/src/config/package.json /usr/bin/blikvm
```

# Reboot

```bash
sudo systemctl reboot
```
