# install kvmd fan
Because the fan model used by BLIKVM hardware is different from that of PIKVM hardware, viewing the fan status through the web interface may fail in the latest system of pikvm. However, this will not affect the normal operation of BLIKVM fan. Blikvm's software system will be launched soon, so there will be no such problems. Therefore, when using the PIKVM image, as long as you ensure that the install script displays OK, it means that the update of the kvmd fan program is successful. The threshold temperature set by the default program is 60 degrees Celsius. When the CPU temperature is detected to exceed 60 degrees Celsius, the fan will start rotating at 85% speed. If you want to modify the threshold temperature and speed, you can modify the kvmd-fan.py script, and then re-execute install.sh.

1、if you use pikvm image, login as root
```
su -
rw
git clone https://github.com/ThomasVon2021/blikvm.git
cd blikvm/package/kvmd-fan
bash install.sh
ro
```

2、if you want to disable fan
```
systemctl disable kvmd-fan
```

3、if you want to start fan
```
systemctl enable kvmd-fan
```

4、if you want to see the fan work status
```
systemctl status kvmd-fan
```
