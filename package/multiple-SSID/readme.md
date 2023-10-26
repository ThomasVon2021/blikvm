# mul-wifi.py is Multiple SSID
First install network-manager.
```
apt install network-manager
systemctl enable NetworkManager
systemctl start NetworkManager
```
Fisrt, you should config the wifi-config.json. Make sure the mul-wifi.py and wifi-config.json in the same path. You can run by manual, like
```
sudo python3 mul-wifi.py
```
Or you can use the following method to join the self-starting when system start.

```
bash install-kvmd-mul-wifi.sh
```

if you want to start now
```
systemctl start kvmd-mul-wifi
```

if you want to see the  work status
```
systemctl status kvmd-mul-wifi
```

if you want to disable 
```
systemctl disable kvmd-mul-wifi
```

