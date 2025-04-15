#ifndef __BLIKVM_CONFIG_H__
#define __BLIKVM_CONFIG_H__

/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2023-06-07 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/

#include "common/blikvm_type.h"
#include "common/blikvm_log/blikvm_log.h"
#include "kvmd/blikvm_oled/blikvm_oled.h"
#include "kvmd/blikvm_switch/blikvm_switch.h"
#include "kvmd/blikvm_fan/blikvm_fan.h"


typedef struct
{
    blikvm_log_t log;
    blikvm_switch_t switch_handle;
    blikvm_oled_config_t oled;
    blikvm_fan_config_t fan;
}blikvm_config_t;

blikvm_int8_t blikvm_config_init();

blikvm_config_t* blikvm_read_config(blikvm_int8_t* file_path);

blikvm_config_t* blikvm_get_config();

#endif