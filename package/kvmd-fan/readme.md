# install kvmd fan
1、if you use pikvm image, the password is *root*
```
su -
rw
git clone --single-branch --branch master https://github.com/ThomasVon2021/pikvm-board.git
cd package/kvmd-fan
bash install.sh
```

2、if you want disable fan，
```
systemctl disable kvmd-fan
```

3、if you want restart fan，
```
systemctl enable kvmd-fan
```

4、if you want see the fan work status
```
systemctl  start kvmd-fan
```