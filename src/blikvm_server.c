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
#include "common/blikvm_log/blikvm_log.h"

#define TAG "SERVER"
/*******************************************************************************
 * private methods defination
 ******************************************************************************/

blikvm_int8_t blikvm_init( blikvm_config_t *config)
{
    blikvm_int8_t ret = -1;
    do
    {
        //1、init log moudle
        blikvm_log_init(&config->log);

        //2、init fan moudle
        blikvm_fan_init();
        //3、init atx moudle
        blikvm_atx_init();

        //4、init oled moudle
        //blikvm_oled_init(config->oled_type);

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

        ret =0;
    }while(0>1);

    return ret;
}
