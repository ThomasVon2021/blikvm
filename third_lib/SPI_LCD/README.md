*** SPI_LCD ***

SPI_LCD is a C library for working with the SPI-connected LCD
displays which use the SSD1351, ST7735, ILI9341 or HX8357 controller chips. The idea is to
provide a simple interface for C programmers to make use of
those low-cost LCD displays without having to read the data sheet or figure
out SPI programming. My main purpose of writing this library is not to
replace the fbtft Linux driver, but to provide a small, efficient way for
programs to talk directly to the LCD controller. My specific use case is
my interest in retro gaming and my desire to get games to run at 60fps
through an interface that only allows up to 33fps via smart buffer
management and hardware scrolling.

![ILI9341 at 60FPS](https://www.youtube.com/watch?v=tKnL1sJpcNo)

Unfortunately the display controller needs a couple of
control lines (GPIO pins) for Reset and Data/Command; this requires making
some changes to the code depending on which GPIO lines you choose to use.
I added the flexibility to use either the SPI/GPIO kernel drivers, or
wiringPi, BCM2835 or PIGPIO. Select the desired library by
using the appropriate #ifdef in spi_lcd.c and in the make_sample makefile.
From my testing, wiringPi is the slowest of the 3 due to its use of the Linux
SPI kernel driver. The advantage to using it is that it doesn't require root
privilege if you set up the SPI kernel driver to be automatically loaded at
boot time. The BCM2835 driver is the next fastest and can potentially use
fewer CPU cycles due to it's use of hardware DMA transfers. PIGPIO is the
fastest of the 3 by a long shot. Part of the reason is that it talks directly
to the SPI shift register; the downside is that it uses more CPU time to send
the data. The included sample code will display the potential maximum frames
per second possible, so you can see how each library performs on your hardware.

Features
--------
- Supports ssd1351 (128x128), st7735 (128x160), ili9341 (240x320) and hx8357 (320x480) displays
- Includes several header pin to GPIO translation tables for non-RPI boards
- Works in both landscape and portrait orientation
- Two sizes of text drawing with custom fg/bg color
- Fast tile drawing for gaming
- Fast stretched tile drawing for scaling 160x144 to 320x216
- Hardware scrolling (vertical/portrait direction only)
- Single pixel drawing
- Backlight on/off
- Portable C code with wrapper functions for I/O; easy to compile on any system
- GPIO wrapper functions for reading/writing in a simple way.
- xpt2046 touch controller input

To build the library, simply type 'make' in the terminal. To build the sample
app, type 'make -f make_sample'. This will build the 'lcd' executable to show
that the library is working.

Copyright (c) 2017 by Larry Bank
Project started 4/25/2017
bitbank@pobox.com<br>

Dev
--------
- If you use nano pi neo, enbale spi1, you need config armbianEnv.txt.
```
vim /boot/armbianEnv.txt
```
Restart after adding the following content. You can get /dev/spidev1.0 after restart.
```
overlays=spi-spidev spi-add-cs1
param_spidev_spi_bus=1
```
Build
--------
- build lib
```
make
```
- build executable program
```
make -f make_sample
```
- run
```
./lcd
```

If you find this code useful, please consider buying me a cup of coffee<br>
[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=SR4F44J2UR8S4)

