/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-09-02 | 0.1       | Thmoasvon     |                 create
 ******************************************************************************/
#include <string.h>

#include "blikvm_server.h"
#include "kvmd/blikvm_fan/blikvm_fan.h"
#include "kvmd/blikvm_atx/blikvm_atx.h"
#include "kvmd/blikvm_oled/blikvm_oled.h"
#include "kvmd/blikvm_switch/blikvm_switch.h"
#include "kvmd/blikvm_rtc/blikvm_rtc.h"
#include "kvmd/blikvm_gpio/blikvm_gpio.h"
#include "common/blikvm_log/blikvm_log.h"

#ifdef  VER4
#include "st7789_oled.h"
#endif

#ifdef TEST_HARDWARE
#include "blikvm_test.h"
#endif

#define TAG "SERVER"
/*******************************************************************************
 * private methods definition
 ******************************************************************************/

static blikvm_int8_t g_switch_init_flag = 0;

blikvm_int8_t blikvm_init( blikvm_config_t *config)
{
    blikvm_int8_t ret = -1;
    do
    {
       if(blikvm_gpio_init() == 0)
        {
            BLILOG_D(TAG,"init gpio success\n");
        }
        else
        {
            BLILOG_E(TAG,"init gpio failed\n");
            break;
        }

        if(blikvm_config_init() == 0)
        {
            BLILOG_D(TAG,"config check success\n");
        }
        else
        {
            BLILOG_E(TAG,"config check failed\n");
        }

        //2. init fan module
        if( blikvm_fan_init() >= 0)
        {
            BLILOG_D(TAG,"init fan success\n");
        }
        else
        {
            BLILOG_E(TAG,"init fan failed\n");
        }

        //3. init atx module
        if(blikvm_atx_init() == 0)
        {
            BLILOG_D(TAG,"init atx success\n");
        }
        else
        {
            BLILOG_E(TAG,"init atx failed\n");
        }

        //4. init switch module
        // if (blikvm_switch_init( &config->switch_handle ) >= 0)
        // {
        //     g_switch_init_flag = 1;
        //     BLILOG_D(TAG,"init switch success\n");
        // }
        // else
        // {
        //     BLILOG_E(TAG,"init switch failed\n");
        // }

        //5. init oled module
        if(blikvm_oled_init(&config->oled) >= 0)
        {
            BLILOG_D(TAG,"init oled success\n");
        }
        else
        {
            BLILOG_E(TAG,"init oled failed\n");
        }

        //5. init rtc module
        blikvm_rtc_init();

    } while (0>1);
    return ret;    
}

blikvm_int8_t blikvm_start(blikvm_config_t *config)
{
    blikvm_int8_t ret = -1;

    do
    {
#ifdef  VER4
        if(blikvm_gpio_start() < 0)
        {
            BLILOG_E(TAG,"gpio start error\n");
        }
        else
        {
            BLILOG_D(TAG,"gpio start ok\n");
        }
#endif
        if(blikvm_fan_start() < 0 )
        {
            BLILOG_E(TAG,"fan start error\n");
            break;
        }
        if (blikvm_atx_start() < 0)
        {
            BLILOG_E(TAG,"atx start error\n");
            break;
        }
        // if( (config->switch_handle.enable > 0) && (g_switch_init_flag > 0))
        // {
        //     if(blikvm_switch_start() < 0)
        //     {
        //         BLILOG_E(TAG,"switch start error\n");
        //         break;
        //     }
        //     else
        //     {
        //         BLILOG_D(TAG,"switch start ok\n");
        //     }
        // }

        if(blikvm_oled_start() < 0)
        {
            BLILOG_E(TAG,"oled start error\n");
            break;
        }
        else
        {
            BLILOG_D(TAG,"oled start ok\n");
        }

#ifdef TEST_HARDWARE
        blikvm_hardware_test();
#endif
        ret =0;
    }while(0>1);

    return ret;
}