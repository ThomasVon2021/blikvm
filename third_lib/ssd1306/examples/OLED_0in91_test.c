/*****************************************************************************
* | File      	:   OLED_0in91_test.c
* | Author      :   Waveshare team
* | Function    :   0.91inch OLED Module test demo
* | Info        :
*----------------
* |	This version:   V2.0
* | Date        :   2020-08-17
* | Info        :
* -----------------------------------------------------------------------------
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "ssd1306_oled.h"
#include "OLED_0in91.h"
#include "common/blikvm_util/blikvm_util.h"
#include "common/blikvm_log/blikvm_log.h"


#define TAG "OLED"
static UBYTE *BlackImage;

blikvm_int32_t blikvm_oled_ssd1306_0in91_init()
{
	blikvm_int32_t ret = -1;
	do
	{
		if(DEV_ModuleInit() != 0) 
		{
			BLILOG_E(TAG,"0.91inch OLED init failed\n");
			break;
		}
		BLILOG_D(TAG,"0.91inch OLED init ok\n");
		OLED_0in91_Init();

		OLED_0in91_Clear();
		
		// 0.Create a new image cache
		UWORD Imagesize = ((OLED_0in91_WIDTH%8==0)? (OLED_0in91_WIDTH/8): (OLED_0in91_WIDTH/8+1)) * OLED_0in91_HEIGHT;
		if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) 
		{
			BLILOG_E(TAG,"Failed to apply for black memory...n");
			return -1;
		}

		Paint_NewImage(BlackImage, OLED_0in91_HEIGHT, OLED_0in91_WIDTH, 90, BLACK);	

		ret = 0;
	} while (0>1);
	return ret;
}

blikvm_int32_t blikvm_oled_ssd1306_0in91_show(void)
{	
	//1.Select Image
	if(BlackImage == NULL)
	{
		BLILOG_E(TAG,"BlackImage is NULL\n");
		return -1;
	}
	Paint_SelectImage(BlackImage);
	DEV_Delay_ms(500);
	Paint_Clear(BLACK);
	int i=0;
	while(1) 
	{
		if(i >=2 )
		{
			OLED_0in91_Clear();
			break;
		}
		// BLILOG_D(TAG,"start show oled info\n");
		// IP address
		if(i%2 == 0 )
		{
			char ip[20]={0};
			GetIP(ip);
			char ip_str[128]={0};
			sprintf(ip_str,"IP:%s",ip);
			Paint_DrawString_EN(0, 0, ip_str, &Font12, WHITE, WHITE);
			// TEMP
			char temp[20]={0};
			sprintf(temp, "temp:%2dF/%2dC",(int)(GetCPUTemp()*1.8 + 32),GetCPUTemp());
			Paint_DrawString_EN(0, 16, temp, &Font12, WHITE, WHITE);
		}
		else
		{
			// CPU LOAD
			char cpu_loading[20]={0};
			sprintf(cpu_loading, "CPU LOAD:%.2f",GetCPULoad());
			Paint_DrawString_EN(10, 0, cpu_loading, &Font12, WHITE, WHITE);

			//Mem 
			char mem_str[128]={0};
			GetMemUsage(mem_str);
			Paint_DrawString_EN(10, 16, mem_str, &Font12, WHITE, WHITE);
		}
		i = i + 1;
        OLED_0in91_Display(BlackImage);
        DEV_Delay_ms(3000); 
        Paint_Clear(BLACK); 		
	}
	return 0;
}

