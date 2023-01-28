/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-09-10 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/

#include "blikvm_oled.h"
#include "common/blikvm_log/blikvm_log.h"
#include <stdlib.h>
#include <stdio.h>

#define TAG "OLED"
static blikvm_oled_type_e g_oled_type;


blikvm_int8_t blikvm_oled_init(blikvm_oled_type_e type)
{
    blikvm_int8_t ret = -1;

    g_oled_type = type;

    return ret;
}


blikvm_int8_t blikvm_oled_start()
{
    blikvm_int8_t ret = -1;
    do
    {
        if( popen("python /usr/bin/blikvm-oled.py &","r") == NULL)
        {
            BLILOG_E(TAG,"don't find oled py\n");
            break;
        }
        ret = 0;
    } while (0>1);

    return ret;
}
