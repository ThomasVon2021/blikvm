#include "common/blikvm_util/blikvm_util.h"
#include "common/blikvm_log/blikvm_log.h"

#include "GUI/LCD_1in3.h"
#include "GUI/GUI_Paint.h"
#include "GUI/GUI_BMP.h"
#include "Fonts/fonts.h"
#include "spi_lcd.h"


#define TAG "OLED"
#define LCD LCD_ST7789
int width=240, height=240;

int oled_240_240_run()
{
	int rc;
	// Nano Pi NEO SPI0 channel
	//rc = spilcdInit(LCD, 0, 0, 40000000, 11, 12, 22); // LCD type, flip 180, SPI Channel, D/C, RST, LED
	// Mango Pi Mcore SPI1 channel
	rc = spilcdInit(LCD, 0, 1, 40000000, 40,22 , 38); // LCD type, flip 180, SPI Channel, D/C, RST, LED
	if(rc != 0)
	{
		BLILOG_E(TAG,"init spi failed:%d\n",rc);
	}

    UWORD *BlackImage;
    UDOUBLE Imagesize = LCD_1IN3_HEIGHT*LCD_1IN3_WIDTH*2;
	BLILOG_D(TAG,"Imagesize = %d\n",Imagesize);
    if((BlackImage = (UWORD *)malloc(Imagesize)) == NULL) 
	{
		BLILOG_E(TAG,"Failed to apply for black memory...\n");
    }
    Paint_NewImage(BlackImage, LCD_1IN3_WIDTH, LCD_1IN3_HEIGHT, 0, WHITE, 16);

	while(1) 
	{
		Paint_Clear(WHITE);
		Paint_SetRotate(ROTATE_180);
        GUI_ReadBmp("oled_info.bmp");
	    Paint_DrawString_EN( 70, 10, "BliKVM", &Font24, WHITE ,BLACK);

		// IP addresss
		char ip[20]={0};
		GetIP(ip);
		Paint_DrawString_EN(60, 55, ip, &Font16, WHITE, BLACK);

		// TEMP
        char temp[20]={0};
        sprintf(temp, "%2dF/%2dC",(int)(GetCPUTemp()*1.8 + 32),GetCPUTemp());
        Paint_DrawString_EN(60, 90, temp, &Font24, WHITE, BLACK);

		//uptime
		char *uptime_str;
		uptime_str = GetUptime();
		Paint_DrawString_EN(60, 135, uptime_str, &Font16, WHITE, BLACK);
		free(uptime_str);

		// CPU LOAD
        char cpu_loading[20]={0};
        sprintf(cpu_loading, "%.2f",GetCPULoad());
        Paint_DrawString_EN(55, 175, cpu_loading, &Font16, WHITE, BLACK);

		//Mem 
		char mem_str[20]={0};
		GetMemUsageShort(mem_str);
		Paint_DrawString_EN(170, 175, mem_str, &Font16, WHITE, BLACK);

        LCD_1IN3_Display(BlackImage);

        sleep(3);
	}
}
