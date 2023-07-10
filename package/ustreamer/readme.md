# Install kvmd ustreamer

## Points to be optimized

When starting ustreamer, you need to start http. If the network card is not successfully started at this time, the binding address will fail, which will lead to the failure of the startup of ustreamer. At present, the strategy is that the startup of ustreamer is delayed for a total of 100 seconds, which is not robust. Everyone has a better solution, welcome to PR.

1. Install the ustream service

```
git clone https://github.com/ThomasVon2021/blikvm.git
cd blikvm/package/ustreamer
sudo bash install-ustreamer.sh
```

2. If you want to disable ustreamer service

```
systemctl disable kvmd-video
```

3. If you want to start ustreamer service

```
systemctl enable kvmd-video
```

4. If you want to see the ustreamer service status

```
systemctl status kvmd-video
```
