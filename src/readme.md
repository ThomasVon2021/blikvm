# build command

## On arm board like raspberry pi use SSD1306 OLED moudle, for BliKVM v1、v2、v3 hardware.
```
make RPI=1 SSD1306=1 
```

## On arm board like H616 borad use ST7789 moudle, for BliKVM V4 hardware.
```
make H616=1 ST7789=1 
```

## On x86 linux
```
make PLATFORM=x86
```