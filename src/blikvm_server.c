/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-09-02 | 0.1       | Thmoasvon     |                 create
 ******************************************************************************/

#include "blikvm_server.h"
#include "kvmd/blikvm_fan/blikvm_fan.h"
#include "kvmd/blikvm_atx/blikvm_atx.h"
#include "kvmd/blikvm_oled/blikvm_oled.h"
#include "kvmd/blikvm_switch/blikvm_switch.h"
#include "common/blikvm_log/blikvm_log.h"
#include "GPIO/armbianio.h"


#define TAG "SERVER"
/*******************************************************************************
 * private methods defination
 ******************************************************************************/
static blikvm_int8_t blikvm_gpio_init();

blikvm_int8_t blikvm_init( blikvm_config_t *config)
{
    blikvm_int8_t ret = -1;
    do
    {
        //1、init log moudle
        blikvm_log_init(&config->log);

       if(blikvm_gpio_init() == 0)
        {
            BLILOG_D(TAG,"init gpio success\n");
        }
        else
        {
            BLILOG_E(TAG,"init gpio failed\n");
            break;
        }

        //2、init fan moudle
        if( blikvm_fan_init() == 0)
        {
            BLILOG_D(TAG,"init fan success\n");
        }
        else
        {
            BLILOG_E(TAG,"init fan failed\n");
        }

        //3、init atx moudle
        if(blikvm_atx_init() == 0)
        {
            BLILOG_D(TAG,"init atx success\n");
        }
        else
        {
            BLILOG_E(TAG,"init atx failed\n");
        }

        //4. init switch mouudle
        blikvm_switch_init();

        //5、init oled moudle
        blikvm_oled_init(config->oled_type);

        //5、init dtc moudle

    } while (0>1);
    return ret;    
}

blikvm_int8_t blikvm_start()
{
    blikvm_int8_t ret = -1;

    do
    {
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
        if(blikvm_switch_start() < 0)
        {
            BLILOG_E(TAG,"switch start error\n");
            break;
        }
        if(blikvm_oled_start() < 0)
        {
            BLILOG_E(TAG,"oled start error\n");
            break;
        }
        else
        {
            BLILOG_D(TAG,"oled start ok\n");
        }


        ret =0;
    }while(0>1);

    return ret;
}

static blikvm_int8_t blikvm_gpio_init()
{
	// Initialize the library
    blikvm_int8_t rc;
    const blikvm_int8_t *szBoardName;
	rc = AIOInit();
	if (rc == 0)
	{
        BLILOG_E(TAG,"Problem initializing ArmbianIO library\n");
		return -1;
	}
	szBoardName = AIOGetBoardName();
    BLILOG_D(TAG,"Running on a %s\n", szBoardName);
    return 0;
}