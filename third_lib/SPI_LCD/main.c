//
// LCD display test program
//
// Copyright (c) 2017 Larry Bank
// email: bitbank@pobox.com
// Project started 5/17/2017
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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <spi_lcd.h>

#include "LCD_1in3.h"
#include "GUI_Paint.h"
#include "GUI_BMP.h"
#include "fonts.h"

unsigned short usColors[8] = {0xf800, 0x7e0, 0x1f, 0xffff, 0xffe0, 0x7ff, 0xf81f, 0x7bef};

//#define LCD LCD_ST7735
//int width=128, height=160;

//#define LCD LCD_SSD1351
//int width=128, height=128;

//#define LCD LCD_ILI9341
//int width=240, height=320;

//#define LCD LCD_ILI9342
//int width=320, height=240;

#define LCD LCD_ST7789
int width=240, height=240;

//#define LCD LCD_HX8357
//int width=320, height=480;

int MilliTime()
{
int iTime;
struct timespec res;

    clock_gettime(CLOCK_MONOTONIC, &res);
    iTime = 1000*res.tv_sec + res.tv_nsec/1000000;

    return iTime;
} /* MilliTime() */

int main(int argc, char* argv[])
{
int rc;
int iTime;
int x, y;

#ifdef TEST_TOUCH_CONTROLLER
int i;

	spilcdInitTouch(TOUCH_XPT2046, 1, 50000);
	spilcdTouchCalibration(149, 1949, 58, 1949);
	for (i=0; i<200; i++)
	{
		rc = spilcdReadTouchPos(&x, &y);
		if (rc != 0)
			printf("x,y = %d,%d\n", x, y);
		usleep(100000);
	}
	spilcdShutdownTouch();
	return 0;

#endif // TEST_TOUCH_CONTROLLER

	// Initialize the library on SPI channel 0
	// The pin numbers are for 40-pin headers on RPi2, RPi3, RPi0
	// Pass it the GPIO pin numbers used for the following:31250000
	// Nano Pi NEO SPI0 channel
	printf("start\n");
	rc = spilcdInit(LCD, 0, 0, 40000000, 11, 12, 22); // LCD type, flip 180, SPI Channel, D/C, RST, LED

    UWORD *BlackImage;
    UDOUBLE Imagesize = LCD_1IN3_HEIGHT*LCD_1IN3_WIDTH*2;
    printf("Imagesize = %d\r\n", Imagesize);
    if((BlackImage = (UWORD *)malloc(Imagesize)) == NULL) 
	{
        printf("Failed to apply for black memory...\r\n");
        exit(0);
    }
    // /*1.Create a new image cache named IMAGE_RGB and fill it with white*/
    Paint_NewImage(BlackImage, LCD_1IN3_WIDTH, LCD_1IN3_HEIGHT, 0, WHITE, 16);
    Paint_Clear(WHITE);
	Paint_SetRotate(ROTATE_180);

    // /* GUI */
    printf("drawing...\r\n");

    // Paint_DrawString_EN(5, 90, "waveshare", &Font20, RED, IMAGE_BACKGROUND);

    // Paint_DrawNum(5, 160, 123456789, &Font20, GREEN, IMAGE_BACKGROUND);
	// Paint_DrawString_CN(5,200, "΢ѩ����",  &Font24CN,IMAGE_BACKGROUND,BLUE);   
        
    // /*3.Refresh the picture in RAM to LCD*/
    // LCD_1IN3_Display(BlackImage);
    //usleep(2000000);

	    // /* show bmp */
	printf("show bmp\r\n");
	GUI_ReadBmp("./pic/oled_info.bmp");
	Paint_DrawString_EN( 70, 10, "BLIKVM", &Font24, WHITE ,BLACK);
	//GUI_ReadBmp("./pic/LCD_1inch3.bmp");
    
    LCD_1IN3_Display(BlackImage);
	usleep(2000000);



	// Nano Pi NEO SPI1 channel
	//rc = spilcdInit(LCD, 0, 1, 40000000, 22, 31, 11); // LCD type, flip 180, SPI Channel, D/C, RST, LED
	// if (rc != 0)
	// {
	// 	printf("Problem initializing spilcd library\n");
	// 	return 0;
	// }
	// printf("spi lcd init ok\n");
    // // Scroll the display while drawing tiles in the landscape orientation
	// spilcdSetOrientation(LCD_ORIENTATION_ROTATED);
	// for (x=0; x<=height; x++)
	// {
	// 	if ((x & 15) == 0)
	// 	{
	// 		for (y=0; y<=width-16; y+= 16)
	// 		{
	// 			spilcdDrawTile(height/2, y, 16, 16, (unsigned char *)usColors, 0);
	// 		}
	// 	}
	// 	spilcdScroll(1, -1);
	// 	usleep(20000);
	// }
	// spilcdFill(255);
	// usleep(2000000);

    // // Measure the maximum screen refresh rate in FPS
	// iTime = MilliTime();
	// for (rc=0; rc<10; rc++)
	// {
	// 	spilcdFill(usColors[rc & 7]);
	// }
	// iTime = MilliTime() - iTime;
    
	// printf("Full screen updates max out at %03.2f FPS\n",10000.0/(float)iTime);
    // // Erase and start over in portrait orientation
    // spilcdFill(0);
    // spilcdSetOrientation(LCD_ORIENTATION_ROTATED);
    // spilcdScrollReset();
    // // Draw some rectangles
    // iTime = MilliTime();
    // for (rc=0; rc<2000; rc++)
    // {
    //    unsigned short usColor;
    //    int w, h, bFill;
    //    usColor = usColors[rand() & 7];
    //    x = rand() & 255;
    //    y = rand() & 511;
    //    w = rand() & 127; 
    //    h = rand() & 127;
    //    bFill = rand() & 1;
    //    spilcdRectangle(x, y, w, h, usColor, bFill);
    // }
    // iTime = MilliTime() - iTime;
    // printf("2000 random rectangles in %d ms\n", iTime);
    // usleep(3000000);

    // spilcdSetOrientation(LCD_ORIENTATION_NATIVE);
    // spilcdFill(0);
    // // Draw scrolling 8x8 text with various background/foreground colors
	// for (rc=0; rc<height; rc++)
	// {
	// 	spilcdScroll(2, 0);
	// 	if ((rc & 3) == 0)
	// 		spilcdWriteString(0, height-8, "This is a test of scrolling text", usColors[(rc>>3)&7], usColors[((rc>>3)+1)&7],0);
	// 	usleep(16000);
	// }
	// spilcdWriteString(0, height/2, "Big Text!", usColors[0], usColors[1],1);
	// usleep(2000000);
    
    // // Change to landscape orientation and draw more text
	// spilcdSetOrientation(LCD_ORIENTATION_ROTATED);
	// spilcdFill(0);
	// for (rc=0; rc<width; rc += 8)
	// {
	// 	spilcdWriteString(0, rc, "90 Degrees rotated text", 0x7e0, 0xf800,0);
	// }
	// spilcdWriteString(0, 0, "Big Rotated Text!", usColors[2], usColors[3],1);
	// usleep(3000000);

    // Quit library and free resources
	spilcdShutdown();

   return 0;
} /* main() */
