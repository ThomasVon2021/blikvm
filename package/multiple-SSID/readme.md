# mul-wifi.py is Multiple SSID

First install & enable network-manager:

```sh
apt install network-manager
systemctl enable NetworkManager
systemctl start NetworkManager
```

Secondly, you should config the wifi-config.json. Make sure the `mul-wifi.py` and `wifi-config.json` are in the same path. You can run it manually, like:

```sh
sudo python3 mul-wifi.py
```

Or you can use the following command to enable the self-starting service on a system start:

```sh
bash install-kvmd-mul-wifi.sh
```

If you want to start the service now:

```sh
systemctl start kvmd-mul-wifi
```

If you want to see the service status:
```
systemctl status kvmd-mul-wifi
```

If you want to disable the service:
```
systemctl disable kvmd-mul-wifi
```

