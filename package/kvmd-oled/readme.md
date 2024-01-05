# install kvmd oled

1、if you use pikvm image, login as root
```
su -
rw
git clone https://github.com/ThomasVon2021/blikvm.git
cd blikvm/package/kvmd-oled
bash install.sh
ro
```

2、if you want to disable oled
```
systemctl disable kvmd-oled
```

3、if you want to start oled
```
systemctl enable kvmd-oled
```

4、if you want to see the fan work status
```
systemctl status kvmd-oled
```
