/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2023-05-28 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/

#include "common/blikvm_util/blikvm_util.h"
#include "common/blikvm_log/blikvm_log.h"
#include "config/blikvm_config.h"
#include <string.h>
#include "GUI/LCD_1in3.h"
#include "GUI/GUI_Paint.h"
#include "GUI/GUI_BMP.h"
#include "Fonts/fonts.h"
#include "spi_lcd.h"

#ifdef TEST_HARDWARE
	#ifdef VER4
#include "blikvm_test.h"
	#endif
#endif


#define TAG "OLED"
#define LCD LCD_ST7789
#define ST7789_V4_DC_PIN 260
#define SHOW_INTERVAL 5U   //uint:s
static UWORD *BlackImage = NULL;

blikvm_int32_t oled_240_240_init()
{

	int rc;
	do
	{
		// Nano Pi NEO SPI0 channel
		//rc = spilcdInit(LCD, 0, 0, 40000000, 11, 12, 22); // LCD type, flip 180, SPI Channel, D/C, RST, LED
		// Mango Pi Mcore SPI1 channel
		rc = spilcdInit(LCD, 0, 1, 40000000, 40,22 , 38); // LCD type, flip 180, SPI Channel, D/C, RST, LED
		if(rc != 0)
		{
			BLILOG_E(TAG,"init spi failed:%d\n",rc);
			break;
		}
		
		UDOUBLE Imagesize = LCD_1IN3_HEIGHT*LCD_1IN3_WIDTH*2;
		BLILOG_D(TAG,"Imagesize = %d\n",Imagesize);
		if((BlackImage = (UWORD *)malloc(Imagesize)) == NULL) 
		{
			BLILOG_E(TAG,"Failed to apply for black memory...\n");
		}
		Paint_NewImage(BlackImage, LCD_1IN3_WIDTH, LCD_1IN3_HEIGHT, 0, WHITE, 16);
	}while (0>1);

	return rc;
}

blikvm_int32_t blikvm_backlight_close()
{
	blikvm_int32_t ret = -1;
	do
	{
		myPinWrite(ST7789_V4_DC_PIN, 0);
		ret = 0;
	}while(0>1);
	return ret;
}

blikvm_int32_t blikvm_backlight_open()
{
	blikvm_int32_t ret = -1;
	do
	{
		myPinWrite(ST7789_V4_DC_PIN, 1);
		ret = 0;
	}while(0>1);
	return ret;
}

blikvm_int32_t oled_240_240_show(blikvm_int32_t diff)
{
	blikvm_int32_t ret = -1;
	blikvm_int32_t i =0;
	do
	{
		if(BlackImage == NULL)
		{
			BLILOG_E(TAG,"BlackImage is null\n");
			break;
		}
		blikvm_backlight_open();
		Paint_Clear(WHITE);
		Paint_SetRotate(ROTATE_270);
        GUI_ReadBmp("/mnt/exec/release/lib/oled_info.bmp");
	    Paint_DrawString_EN( 60, 10, "BliKVM V4", &Font24, WHITE ,BLACK);

		// IP addresss
		char ip[20]={0};
		GetIP(ip);
		Paint_DrawString_EN(60, 55, ip, &Font16, WHITE, BLACK);

		// Seccond IP address
		char* secondIPPtr = blikvm_get_config()->oled.secondIP;
		if( secondIPPtr != NULL && strlen(secondIPPtr) > 0){
			char secondIp[20]={0};
			if(GetSpecificIP(secondIPPtr,secondIp) == 0){
				Paint_DrawString_EN(60, 90, secondIp, &Font16, WHITE, BLACK);
			}
		}

		// TEMP
        char temp[20]={0};
        sprintf(temp, "%2dF/%2dC",(int)(GetCPUTemp()*1.8 + 32),GetCPUTemp());
        Paint_DrawString_EN(60, 135, temp, &Font24, WHITE, BLACK);

		//uptime
		char *uptime_str;
		uptime_str = GetUptime();
		Paint_DrawString_EN(60, 175, uptime_str, &Font16, WHITE, BLACK);
		free(uptime_str);

		// CPU LOAD
        char cpu_loading[20]={0};
        sprintf(cpu_loading, "%.2f",GetCPULoad());
        Paint_DrawString_EN(55, 210, cpu_loading, &Font16, WHITE, BLACK);

		//Mem 
		char mem_str[20]={0};
		GetMemUsageShort(mem_str);
		Paint_DrawString_EN(170, 210, mem_str, &Font16, WHITE, BLACK);

#ifdef TEST_HARDWARE
	#ifdef VER4
		blikvm_test_result_t* test_result;
		test_result = blikvm_get_test_result();
		char test_str[20]= {0};
		if(test_result->test_flag == 0)
		{
			Paint_DrawString_EN(60, 90, "Testing", &Font24, WHITE, RED);
		}
		else if(test_result->uart_flag == 0)
		{
			BLILOG_E(TAG,"uart:%d, test failed:%d\n",test_result->uart_flag, test_result->code);
			sprintf(test_str, "uart fail: %d",test_result->code);
			Paint_DrawString_EN(10, 90, test_str, &Font24, WHITE, RED);
		}else if(test_result->rtc_flag == 0)
		{
			sprintf(test_str, "rtc fail: %d",test_result->code);
			Paint_DrawString_EN(10, 90, test_str, &Font24, WHITE, RED);
		}else if(test_result->wifi_flag == 0)
		{
			sprintf(test_str, "wifi fail: %d",test_result->code);
			Paint_DrawString_EN(10, 90, test_str, &Font24, WHITE, RED);
		}else if(test_result->atx_flag == 0)
		{
			sprintf(test_str, "atx fail: %d",test_result->code);
			Paint_DrawString_EN(10, 90, test_str, &Font24, WHITE, RED);
		}else
		{
			Paint_DrawString_EN(60, 90, "test OK!", &Font24, WHITE, GREEN);
		}
	#endif
#endif

        LCD_1IN3_Display(BlackImage);

		sleep(SHOW_INTERVAL);
		BLILOG_D(TAG,"close backlight\n");
		blikvm_backlight_close();
		ret = 0;
		i = i + 1;
	}while(i < (diff/5));
	
	return ret;
}


