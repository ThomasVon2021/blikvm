# install kvmd ustreamer
## Points to be optimized
When starting ustreamer, you need to start http. If the network card is not successfully started at this time, the binding address will fail, which will lead to the failure of the startup of ustreamer. At present, the strategy is that the startup of ustreamer is delayed for a total of 21 seconds, which is not robust. Everyone has a better solution, welcome to PR.

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
systemctl disable kvmd-video
```

3、if you want to start fan
```
systemctl enable kvmd-video
```

4、if you want to see the fan work status
```
systemctl status kvmd-video
```