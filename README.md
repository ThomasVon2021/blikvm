<h1 align="center">BliKVM - Integrated software and hardware solution for KVM</h1>
<p align="center">
<a href="README_CN.md">中文说明</a>
</p>
<p align="center">
  <a href="https://www.blicube.com">BliKVM</a> is an open-source KVM, it has four versions: v1 CM4,  v2 PCIe, v3 HAT, v4 Allwinner. This device helps to manage servers or workstations remotely, regardless of the health of the operating system or whether one is installed. You can fix any problem, configure the BIOS, and even reinstall the OS using the virtual CD-ROM or Flash Drive. Unlike software-based remote management, you don't need to install any software on the controlled computer for non-intrusive control.
</p>

<p align="center">
  <a href="https://github.com/ThomasVon2021/blikvm/stargazers">
    <img alt="GitHub Stars" src="https://img.shields.io/github/stars/ThomasVon2021/blikvm?color=ffcb2f&label=%E2%AD%90%20on%20GitHub">
  </a>
  <a href="https://discord.gg/9Y374gUF6C">
    <img alt="Discord Server" src="https://img.shields.io/discord/943534043515977768?color=0&label=discord%20server&logo=discord">
  </a>
</p>

![](/images/hardware-connect.png)
## Overview

| __中文界面__ | __English UI__ |
|--------------------------------------------|-------------------------------------------|
| ![chinese](/images/web/web-chinese.png) | ![English UI](/images/web/web-english.png) |


## Features

- **Video capture** (HDMI/DVI/VGA)
- **Keyboard forwarding**
- **Mouse forwarding**
- **Mass Storage Drive**
- **ATX** Control the server power using ATX functions
- Wake-on-LAN
- **Fullscreen mode**
- Access via **Web UI**
- Support **multi language** switching
- **Health monitoring** of the Pi
- PWM **FAN** controller
- Support **PoE**
- **Serial** console port
- **I2C** display connector
- **Real Time Clock (RTC)**  

## Support

- Check out [the documentation](https://wiki.blicube.com/blikvm/)!
- Join the [Discord Community Chat](https://discord.gg/9Y374gUF6C) for news, questions and support!
- Download [BLIKVM OS](https://wiki.blicube.com/blikvm/en/flashing_os/).

## Purchase
- [Purchase - BliKVM CM4 v1 Version](https://www.aliexpress.com/item/1005003262886521.html)
- [Purchase - BliKVM HAT v3  Version](https://www.aliexpress.com/item/1005004377930400.html)
- [Purchase - BliKVM PCIe v2 version](https://www.aliexpress.com/item/1005004572837650.html)
- [Purchase - BliKVM Allwinner v4 Version](https://www.aliexpress.com/item/3256805673100994.html)

## Other Purchase Links
- [Purchase - PiKVM ATX Power Adaptor](https://www.aliexpress.com/item/1005003761450893.html)
- [Purchase - PiKVM USB Splitter](https://www.aliexpress.com/item/1005003793429781.html)
- [Purchase - PiKVM HDMI to CSI Board](https://www.aliexpress.com/item/1005002861310912.html)
- [Purchase - BliKVM KVM switch(4 port) v1.0](https://www.aliexpress.com/item/1005005149590635.html)
- [Purchase - BliKVM KVM+ATX switch(8 port) v2.0](https://www.aliexpress.com/item/3256807838289113.html?gatewayAdapt=4itemAdapt)

## Videos
- [Watch - Blicube BliKVM V4: Is This the Best Linux IP KVM Yet?](https://www.youtube.com/watch?v=sbH9-fmWBS8)
- [Watch - Goodbye IPMI - Blicube BliKVM V4 Review](https://www.youtube.com/results?search_query=blikvm)
- [Watch - CM4 version Review by `Geerling Engineering`](https://www.youtube.com/watch?v=3OPd7svT3bE) 
- [Watch - PCIe version Review by `Geerling Engineering`](https://www.youtube.com/watch?v=cVWF3u-y-Zg) 
- [Watch - Feature Overview for CM4 Version](https://www.youtube.com/watch?v=aehOawHklGE)
- [Watch - Unboxing for CM4 Version](https://www.youtube.com/watch?v=d7I9l5yG5M8)

## Hardware version

![Image title](/images/version_all.png)

## Developer
Front and backend separation software architecture: 
![](/images/docs_image/arch.drawio.png)
See more in the [dev-readme](dev-readme.md) file.

## Add a new language
  If you want to add your country's language on the web interface, you can submit the corresponding PR to this path based on the English version of the [path](https://github.com/ThomasVon2021/blikvm-web-client/tree/master/src/utils/locales). After your PR is merged, we will add your language in subsequent versions of the release.If you want to conduct testing before submitting the PR, you can refer to this [document](https://github.com/ThomasVon2021/blikvm-web-client/blob/master/readme.md) and test it on your computer.

## Reporting a Bug & Requesting a New Feature
  If you encounter an issue while using the project or discover a bug, or you have a new feature request or improvement suggestion, please raise an issue. We welcome your suggestions as they help us continuously improve the project and cater to more user needs.

## Donate
This project is developed by Open Source enthusiasts. If you find BliKVM useful or it has saved you a long trip to check on an unresponsive server, you can support us by donating a few dollars on [Patreon](https://www.patreon.com/blikvm) or [Paypal](https://www.paypal.me/blikvm) or buying our devices. With this money, we will be able to buy new hardware (Raspberry Pi boards and other components) to test and maintain various configurations of BliKVM, and generally devote significantly more time to the project. At the bottom of this page are the names of all the people who have helped this project develop with their donations. Our gratitude knows no bounds!

If you wish to use BliKVM in production, we accept orders to modify it for your needs or implement custom features you require. Contact us via [live chat](https://discord.gg/9Y374gUF6C) or email: info@blicube.com.

## Acknowledgments

BliKVM would not be possible without the excellent and generous work from many open source projects, 
the most notable of which are listed below:  

- [PiKVM](https://github.com/pikvm/pikvm)
- [TinyPilot](https://github.com/tiny-pilot/tinypilot)

  -----
## License
Copyright (C) 2018-2024 by blicube info@blicube.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see https://www.gnu.org/licenses/.
