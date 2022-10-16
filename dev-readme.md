# base raspberry system
- Raspberry Pi OS Lite
- Release date: September 22nd 2022
- System: 32-bit
- Kernel version: 5.15
- Debian version: 11 (bullseye)

# Main 
```
sudo -i

#install wiringpi
cd /tmp
wget https://project-downloads.drogon.net/wiringpi-latest.deb
sudo dpkg -i wiringpi-latest.deb

# install Adafruit-SSD1306
pip install adafruit-circuitpython-ssd1306

mkdir /opt/bin
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
make WITH_JANUS=1

The WITH_JANUS option compiles the µStreamer Janus plugin and outputs it to janus/libjanus_ustreamer.so. Move this file to the plugin directory of your Janus installation.

mv \
  janus/libjanus_ustreamer.so \
  /opt/janus/lib/janus/plugins/libjanus_ustreamer.so
Finally, specify a qualifier for the shared memory object so that the µStreamer Janus plugin can read µStreamer's video data.

cat > /opt/janus/lib/janus/configs/janus.plugin.ustreamer.jcfg <<EOF
memsink: {
  object = "demo::ustreamer::h264"
}
EOF
```

# web
apt install -y vim libcurl4-openssl-dev
apt install -y  php7.4-bz2 php7.4-cli php7.4-curl php7.4-dev php7.4-json php7.4-mbstring php7.4-xml php7.4-zip
vim  /etc/php/7.4/cli/php.ini
date.timezone = Asia/Shanghai
apt install php-pear 
pecl install http://pecl.php.net/get/swoole-4.8.9.tgz
#all is yes
echo "extension=swoole.so" > /etc/php/7.4/cli/conf.d/swoole.ini
vim /etc/php/7.4/cli/php.ini
extension=swoole.so

pecl install inotify
vim /etc/php/7.4/cli/php.ini
extension=inotify.so

## webclient
```
npm install
npm run serve
npm run build
```

# Hid config
```
cd blikvm/package/kvmd-hid/
bash install.sh
```