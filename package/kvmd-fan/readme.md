# install kvmd fan
1、if you use pikvm image, login as root
```
su -
rw
git clone https://github.com/ThomasVon2021/blikvm.git
cd package/kvmd-fan
bash install.sh
```

2、if you want disable fan，
```
systemctl disable kvmd-fan
```

3、if you want start fan，
```
systemctl enable kvmd-fan
```

4、if you want see the fan work status
```
systemctl  status kvmd-fan
```
