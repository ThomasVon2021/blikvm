# install kvmd janus

1、if you use pikvm image, login as root
```
su -
rw
git clone https://github.com/ThomasVon2021/blikvm.git
cd blikvm/package/ustreamer
bash install.sh
ro
```

2、if you want to disable fan
```
systemctl disable kvmd-janus
```

3、if you want to start fan
```
systemctl enable kvmd-janus
```

4、if you want to see the fan work status
```
systemctl status kvmd-janus
```