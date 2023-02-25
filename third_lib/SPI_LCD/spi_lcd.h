#ifndef SPI_LCD_H
#define SPI_LCD_H
//
// SPI_LCD using the SPI interface
// Copyright (c) 2017 Larry Bank
// email: bitbank@pobox.com
// Project started 4/25/2017
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
typedef enum
{
 MODE_DATA = 0,
 MODE_COMMAND
} DC_MODE;

// Sets the D/C pin to data or command mode
void spilcdSetMode(int iMode);

//
// Choose the gamma curve between 2 choices (0/1)
// ILI9341 only
//
int spilcdSetGamma(int iMode);

// Initialize the library
int spilcdInit(int iLCDType, int bFlipped, int iSPIChannel, int iSPIFreq, int iDCPin, int iResetPin, int iLEDPin);

//
// Initialize the touch controller
//
int spilcdInitTouch(int iType, int iChannel, int iSPIFreq);

//
// Set touch calibration values
// These are the minimum and maximum x/y values returned from the sensor
// These values are used to normalize the position returned from the sensor
//
void spilcdTouchCalibration(int iminx, int imaxx, int iminy, int imaxy);

//
// Shut down the touch interface
//
void spilcdShutdownTouch(void);

//
// Read the current touch values
// values are normalized to 0-1023 range for x and y
// returns: -1=not initialized, 0=nothing touching, 1=good values
//
int spilcdReadTouchPos(int *pX, int *pY);

// Turns off the display and frees the resources
void spilcdShutdown(void);

// Fills the display with the byte pattern
int spilcdFill(unsigned short usPattern);

//
// Draw a rectangle and optionally fill it
//
void spilcdRectangle(int x, int y, int w, int h, unsigned short usColor, int bFill);

//
// Reset the scroll position to 0
//
void spilcdScrollReset(void);

// Configure a GPIO pin for input
// Returns 0 if successful, -1 if unavailable
int spilcdConfigurePin(int iPin);

// Read from a GPIO pin
int spilcdReadPin(int iPin);

//
// Scroll the screen N lines vertically (positive or negative)
// This is a delta which affects the current hardware scroll offset
// If iFillcolor != -1, the newly exposed lines will be filled with that color
//
void spilcdScroll(int iLines, int iFillColor);

//
// Draw a NxN tile scaled 150% in both directions
int spilcdDrawTile150(int x, int y, int iTileWidth, int iTileHeight, unsigned char *pTile, int iPitch);

// Draw a NxN tile
int spilcdDrawTile(int x, int y, int iTileWidth, int iTileHeight, unsigned char *pTile, int iPitch);

// Draw a 16x16 tile with variable cols/rows removed
int spilcdDrawMaskedTile(int x, int y, unsigned char *pTile, int iPitch, int iColMask, int iRowMask);

// Draw a NxN tile scaled to 2x width, 1.5x height with pixel averaging
int spilcdDrawScaledTile(int x, int y, int cx, int cy, unsigned char *pTile, int iPitch);

int spilcdDraw53Tile(int x, int y, int cx, int cy, unsigned char *pTile, int iPitch);

// Draw a 16x16 tile as 16x13 (with priority to non-black pixels)
int spilcdDrawRetroTile(int x, int y, unsigned char *pTile, int iPitch);

// Draw a 16x16 tile scaled to 16x14 with pixel averaging
int spilcdDrawSmallTile(int x, int y, unsigned char *pTile, int iPitch);

// Write a text string to the display at x (column 0-83) and y (row 0-5)
// bLarge = 0 - 8x8 font, bLarge = 1 - 16x24 font
int spilcdWriteString(int x, int y, char *szText, unsigned short usFGColor, unsigned short usBGColor, int bLarge);

// Write a text string of 8x8 characters
// quickly to the LCD with a single data block write.
// This is necessary because there is a lot of latency between
// writes when using the spidev kernel driver
int spilcdWriteStringFast(int x, int y, char *szText, unsigned short usFGColor, unsigned short usBGColor);

// Sets a pixel to the given color
// Coordinate system is pixels, not text rows (0-239, 0-319)
int spilcdSetPixel(int x, int y, unsigned short usPixel);

// Set the software orientation
int spilcdSetOrientation(int iOrientation);

//
// Treat the LCD as a 240x320 portrait-mode image
// or a 320x240 landscape mode image
// This affects the coordinate system and rotates the
// drawing direction of fonts and tiles
//
#define LCD_ORIENTATION_NATIVE 1
#define LCD_ORIENTATION_ROTATED 2

#define LCD_ILI9341 1
#define LCD_HX8357 2
#define LCD_ST7735 3
#define LCD_SSD1351 4
#define LCD_ILI9342 5
#define LCD_ST7789 6

// touch panel types
#define TOUCH_XPT2046 1

#endif // SPI_LCD_H
