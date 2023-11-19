/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-09-10 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/

#include "blikvm_oled.h"
#include "common/blikvm_log/blikvm_log.h"
#include "common/blikvm_util/blikvm_util.h"
#include "config/blikvm_config.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#ifdef SSD1306
#include "ssd1306_oled.h"
#endif
#ifdef ST7789
#include "st7789_oled.h"
#endif


#define TAG "OLED"
static blikvm_oled_type_e g_oled_type;
static blikvm_int8_t g_init_flag=-1;

static blikvm_void_t *blikvm_oled_loop(void *_);


blikvm_int8_t blikvm_oled_init(blikvm_oled_config_t* config)
{
    blikvm_int8_t ret = -1;

    do
    {
        blikvm_board_type_e type = blikvm_get_board_type();

        if (type == UNKONW_BOARD)
        {
            BLILOG_E(TAG,"Unsupported board\n");
            break;
        }
        
        switch (type)
        {
        case PI4B_BOARD:
            g_oled_type = OLED_SSD1306_128_32;
            break;
        case CM4_BOARD:
            g_oled_type = OLED_SSD1306_128_64;
            break;
        case H616_BOARD:
            g_oled_type = OLED_ST7789_240_240;
            break;
        default:
            break;
        }
        BLILOG_D(TAG,"board:%d oled:%d\n",type, g_oled_type);

        switch (g_oled_type)
        {
        #ifdef SSD1306
            case OLED_SSD1306_128_32:
                blikvm_oled_ssd1306_0in91_init();
                break;
            case OLED_SSD1306_128_64:
                blikvm_oled_ssd1306_0in96_init();
                break;
        #endif
        #ifdef ST7789
            case OLED_ST7789_240_240:
                oled_240_240_init();
                break;
        #endif
            default:
                break;
        }

        g_init_flag = 1;
        BLILOG_D(TAG,"init oled success\n");
        ret = 0;
    } while (0>1);
    
    return ret;
}


blikvm_int8_t blikvm_oled_start()
{
    blikvm_int8_t ret = -1;
    pthread_t blikvm_oled_thread;
    do
    {
        if(g_init_flag != 1U)
        {
            BLILOG_E(TAG,"not init\n");
            break;
        }
        blikvm_int8_t thread_ret = pthread_create(&blikvm_oled_thread, NULL, blikvm_oled_loop, NULL);
        if(thread_ret != 0)
        {
            BLILOG_E(TAG,"creat loop thread failed\n");
            break;
        }
        ret = 0;
    } while (0>1);

    return ret;
}

static blikvm_void_t *blikvm_oled_loop(void *_)
{
    
    blikvm_oled_config_t* oled_config = &(blikvm_get_config()->oled) ;
    while(1)
    {
        blikvm_int32_t uptime = skdy_get_int_uptime();  //unit:min
        if((uptime > MIN_START_SHOW_TIME) && (uptime> oled_config->restart_show_time) && (oled_config->interval_display_time > 0))
        {
            BLILOG_D(TAG,"oled sleep time:%d\n",oled_config->interval_display_time * 60 );
            sleep(oled_config->interval_display_time * 60);
        }
        switch (g_oled_type)
        {
    #ifdef SSD1306
        case OLED_SSD1306_128_32:
            blikvm_oled_ssd1306_0in91_show();
            break;
        case OLED_SSD1306_128_64:
            blikvm_oled_ssd1306_0in96_show();
            break;
    #endif
    #ifdef ST7789
        case OLED_ST7789_240_240:
            oled_240_240_show();
            break;
    #endif
        default:
            break;
        }

    }

    return NULL;
}

blikvm_int8_t blikvm_oled_close()
{
    blikvm_int8_t ret = -1;
    do
    {
        if(g_init_flag != 1U)
        {
            BLILOG_E(TAG,"not init\n");
            break;
        }
        
        ret = 0;
    } while (0>1);
    return ret;
}
