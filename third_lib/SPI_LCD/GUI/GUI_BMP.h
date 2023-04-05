/*****************************************************************************
* | File      	:   GUI_BMP.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master 
*                and enhance portability
*----------------
* |	This version:   V1.0
* | Date        :   2018-01-11
* | Info        :   Basic version
*
******************************************************************************/
#ifndef __GUI_BMP_H
#define __GUI_BMP_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include "GUI_Paint.h"

#define  RGB(r,g,b)         (((r>>3)<<11)|((g>>2)<<5)|(b>>3))


/****************************** Bitmap standard information*************************************/
/*Bitmap file header   14bit*/
typedef struct BMP_FILE_HEADER {
    UWORD bType;                 //File identifier
    UDOUBLE bSize;                //The size of the file
    UWORD bReserved1;            //Reserved value, must be set to 0
    UWORD bReserved2;            //Reserved value, must be set to 0
    UDOUBLE bOffset;              //The offset from the beginning of the file header to the beginning of the image data bit
} __attribute__ ((packed)) BMPFILEHEADER;    // 14bit

/*Bitmap information header  40bit*/
typedef struct BMP_INFO {
    UDOUBLE bInfoSize;            //The size of the header
    UDOUBLE bWidth;               //The width of the image
    UDOUBLE bHeight;              //The height of the image
    UWORD bPlanes;               //The number of planes in the image
    UWORD bBitCount;             //The number of bits per pixel
    UDOUBLE bCompression;         //Compression type
    UDOUBLE bmpImageSize;         //The size of the image, in bytes
    UDOUBLE bXPelsPerMeter;       //Horizontal resolution
    UDOUBLE bYPelsPerMeter;       //Vertical resolution
    UDOUBLE bClrUsed;             //The number of colors used
    UDOUBLE bClrImportant;        //The number of important colors
} __attribute__ ((packed)) BMPINF;

/*Color table: palette */
typedef struct RGB_QUAD {
    UBYTE rgbBlue;               //Blue intensity
    UBYTE rgbGreen;              //Green strength
    UBYTE rgbRed;                //Red intensity
 //   UBYTE rgbReversed;           //Reserved value
} __attribute__ ((packed)) RGBQUAD;

typedef struct ARGB_QUAD {
    UBYTE rgbBlue;               //Blue intensity
    UBYTE rgbGreen;              //Green strength
    UBYTE rgbRed;                //Red intensity
	UBYTE a;           //Reserved value
} __attribute__ ((packed)) ARGBQUAD;
/**************************************** end ***********************************************/

UBYTE GUI_ReadBmp(const char *path);
#endif