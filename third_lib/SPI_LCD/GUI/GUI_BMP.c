/*****************************************************************************
* | File      	:   BMP_APP.c
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
#include "GUI_BMP.h"
#include <stdio.h>	//fseek fread
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>	//memset

#include "GUI_Paint.h"
// #include "GUI_Cache.h"


UBYTE GUI_ReadBmp(const char *path)
{
    FILE *fp;                     //Define a file pointer 
    BMPFILEHEADER bmpFileHeader;  //Define a bmp file header structure
    BMPINF bmpInfoHeader;         //Define a bmp bitmap header structure 
	//UBYTE rgbReversed;
    // Binary file open
    if((fp = fopen(path, "rb")) == NULL) { 	// fp = 0x00426aa0
        DEBUG("Cann't open the file!\n");
        return 0;
    }
    //printf("open:%s \n",path);
    
    fseek(fp, 0, SEEK_SET); 								//Set the file pointer from the beginning	
    fread(&bmpFileHeader, sizeof(BMPFILEHEADER), 1, fp);	//Gets the BMP file header
	fread(&bmpInfoHeader, sizeof(BMPINF), 1, fp);			//Gets BMP Info Header
	//printf("bBitCount=%d \n",bmpInfoHeader.bBitCount);		//Print a pixel takes several bit!!!
	
	
	//Get palette information, Max 256 color information
	//You can use the malloc function to reduce the memory required to run this program
	/*
			bBitCount	RGBPAD
				1		   2
				4		  16
				8		 256
	 */
	RGBQUAD RGBPAD[256];	
	if(bmpInfoHeader.bBitCount<16)
	{	
		
		uint16_t i=0;
		for(i=0;i<(1<<(bmpInfoHeader.bBitCount));i++)
		{
			fseek(fp,(0X36)+(i*4), SEEK_SET);
			fread(&RGBPAD[i], sizeof(RGBQUAD), 1, fp);			
		}
	}
	
	//In Windows each row data must be divisible by 4 byte
	//Calculate how many bytes per row of data need to be indented

	int32_t  indent;
	if(bmpInfoHeader.bBitCount<8)	indent=(4-((bmpInfoHeader.bWidth/(8/bmpInfoHeader.bBitCount))%4))%4;
	else 							indent=(4-((bmpInfoHeader.bWidth*(bmpInfoHeader.bBitCount/8))%4))%4;
	
	
	int 		row, col; 	
    short 		data;			//All data formats are converted to RGB565 format
	char		pixels,temp;		
	int len  = 	bmpInfoHeader.bBitCount / 8;
	ARGBQUAD 	argb;
	/*
	fseek(fp, bmpFileHeader.bOffset, SEEK_SET);
	printf("frist add:0x%x \r\n",bmpFileHeader.bOffset);
	printf("BMP height:%d\r\n",bmpInfoHeader.bHeight);
	printf("BMP width :%d\r\n",bmpInfoHeader.bWidth);
	*/
	// get bmp data and show
	for(row = 0; row < bmpInfoHeader.bHeight;row++) 
	{		
		for(col = 0; col < bmpInfoHeader.bWidth; ) 
		{
			if(bmpInfoHeader.bBitCount==16)
			{
				if(fread((char *)&data, 1, len, fp) != len)
				{
					perror("get bmpdata:\r\n");
					break;
				}  
				//ARGB4444 format cannot be recognized for the time being. It can only be used to identify RGB565 format information!!
				if(bmpInfoHeader.bInfoSize==0x38)
				{	
					Paint_SetPixel(col, bmpInfoHeader.bHeight - row - 1, data);
				}
				//Used to identify the XRGB1555 format
				else if((bmpInfoHeader.bInfoSize==0x28)&&(bmpInfoHeader.bCompression==0x00))
				{
					data=((((long)((data>>5)&0x1f)*0X3F)/0X1F)<<5)+(data&0x1f)+((data&0xEC00)<<1);
					Paint_SetPixel(col, bmpInfoHeader.bHeight - row - 1, data);
				}
				col++;
			}
			//For RGB888 ARGB8888 XRGB8888 format uniform compression and removal of alpha
			else if(bmpInfoHeader.bBitCount>16)
			{
				if(fread((char *)&argb, 1, len, fp) != len)
				{
					printf("x:%d,y:%d\r\n",col,row);
					perror("get bmpdata: \r\n");
					break;
				}
				data = RGB((argb.rgbRed), (argb.rgbGreen), (argb.rgbBlue));
				Paint_SetPixel(col, bmpInfoHeader.bHeight - row - 1, data);
				col++;
			}	
			//bBitCount<8 format
			else		
			{
				if(fread((char *)&pixels, 1, 1, fp) != 1)
				{
					perror("get bmpdata:\r\n");
					break;
				}
				for(temp=0;(temp<(8/bmpInfoHeader.bBitCount))&&(col < bmpInfoHeader.bWidth);temp++,col++)
				{
					if(bmpInfoHeader.bBitCount==1){
						data=(pixels<<temp)>>7;
						data&=0x01;
					}
					else if(bmpInfoHeader.bBitCount==4){	
						data=(pixels<<(temp*4))>>4;
						data&=0x0f;					
					}
					else {data=pixels;}
					data=RGB((RGBPAD[data].rgbRed), (RGBPAD[data].rgbGreen), (RGBPAD[data].rgbBlue));					
					Paint_SetPixel(col, bmpInfoHeader.bHeight - row - 1, data);
				}				
			}	
		}
		//indent!4 byte alignment
		fseek(fp, indent, SEEK_CUR);
	}
	
	fclose(fp);
    return 0;
}