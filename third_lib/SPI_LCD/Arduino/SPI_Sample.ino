//
// LCD display test program
//
// Copyright (c) 2017 Larry Bank
// email: bitbank@pobox.com
// Project started 5/17/2017
//
// Modified By Paul Conti 02/28/2019 for Arduino DUE using ILI9341 TFT driver
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//
#include <Arduino.h>
#include <SPI.h>
#include "spi_lcd.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

unsigned short usColors[8] = {0xf800, 0x7e0, 0x1f, 0xffff, 0xffe0, 0x7ff, 0xf81f, 0x7bef};

//#define LCD LCD_ST7735
//int width=128, height=160;

//#define LCD LCD_SSD1351
//int width=128, height=128;

#define LCD LCD_ILI9341
int width=240, height=320;

//#define LCD LCD_ILI9342
//int width=320, height=240;

//#define LCD LCD_ST7789
//int width=240, height=240;

//#define LCD LCD_HX8357
//int width=320, height=480;

#if defined (ARDUINO_ARCH_ARC32) || defined (ARDUINO_MAXIM)
  #define SPI_DEFAULT_FREQ  16000000
#elif defined (__AVR__) || defined(TEENSYDUINO)
  #define SPI_DEFAULT_FREQ  8000000
#elif defined(ESP8266) || defined(ESP32)
  #define SPI_DEFAULT_FREQ  40000000
#elif defined(RASPI)
  #define SPI_DEFAULT_FREQ  80000000
#elif defined(ARDUINO_ARCH_STM32F1)
  #define SPI_DEFAULT_FREQ  36000000
#else
  #define SPI_DEFAULT_FREQ  24000000  ///< Default SPI data clock frequency
#endif

void setup()
{
  int rc;

	// Initialize the library on SPI channel 0
	// The pin numbers are for 40-pin headers on RPi2, RPi3, RPi0
	// Pass it the GPIO pin numbers used for the following:
	rc = spilcdInit(LCD, 0, SPI_DEFAULT_FREQ, 10, 9, -1, 13); // LCD type, flip 180, SPI Channel, D/C, RST, LED
	if (rc != 0)
	{
		while(1) {};
	}

}

void loop() {
  int rc;
  int iTime;
  int x, y;

  // Scroll the display while drawing tiles in the landscape orientation
	spilcdSetOrientation(LCD_ORIENTATION_ROTATED);
	for (x=0; x<=height; x++)
	{
		if ((x & 15) == 0)
		{
			for (y=0; y<=width-16; y+= 16)
			{
				spilcdDrawTile(height/2, y, 16, 16, (unsigned char *)usColors, 0);
			}
		}
		spilcdScroll(1, -1);
		delay(20);
	}
	
	delay(2000);

    // Measure the maximum screen refresh rate in FPS
	iTime = millis();
	for (rc=0; rc<10; rc++)
	{
		spilcdFill(usColors[rc & 7]);
	}
	iTime = millis() - iTime;
    
  // Erase and start over in portrait orientation
  spilcdFill(0);
  spilcdSetOrientation(LCD_ORIENTATION_ROTATED);
  spilcdScrollReset();
  // Draw some rectangles
  iTime = millis();
  for (rc=0; rc<200; rc++)
  {
     unsigned short usColor;
     int w, h, bFill;
     usColor = usColors[rand() & 7];
     x = rand() & 255;
     y = rand() & 511;
     w = rand() & 127; 
     h = rand() & 127;
     bFill = rand() & 1;
     spilcdRectangle(x, y, w, h, usColor, bFill);
  }
  iTime = millis() - iTime;
  delay(3000);

  spilcdSetOrientation(LCD_ORIENTATION_NATIVE);
  spilcdFill(0);
  // Draw scrolling 8x8 text with various background/foreground colors
	for (rc=0; rc<height; rc++)
	{
		spilcdScroll(2, 0);
		if ((rc & 3) == 0)
			spilcdWriteString(0, height-8, "This is a test of scrolling text", usColors[(rc>>3)&7], usColors[((rc>>3)+1)&7],0);
		delay(16);
	}
	spilcdWriteString(0, height/2, "Big Text!", usColors[0], usColors[1],1);
	delay(2000);
    
    // Change to landscape orientation and draw more text
	spilcdSetOrientation(LCD_ORIENTATION_ROTATED);
	spilcdFill(0);
	for (rc=0; rc<width; rc += 8)
	{
		spilcdWriteString(0, rc, "90 Degrees rotated text", 0x7e0, 0xf800,0);
	}
	spilcdWriteString(0, 0, "Big Rotated Text!", usColors[2], usColors[3],1);
	delay(3000);

    // Quit library and free resources
	spilcdShutdown();

  while(1) {}
} /* main() */
