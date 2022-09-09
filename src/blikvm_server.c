/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-09-02 | 0.1       | Thmoasvon     |                 create
 ******************************************************************************/

#include "blikvm_server.h"
#include "kvmd/blikvm_fan/blikvm_fan.h"


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

        //4、init oled moudle

        //5、init dtc moudle

    } while (0>1);
    return ret;    
}