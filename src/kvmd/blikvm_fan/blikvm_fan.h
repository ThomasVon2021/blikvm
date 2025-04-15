#ifndef __BLIKVM_FAN_H__
#define __BLIKVM_FAN_H__

/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-09-05 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/

#include "common/blikvm_log/blikvm_log.h"

typedef struct
{
    blikvm_int32_t threshold;
}blikvm_fan_config_t;

blikvm_int8_t blikvm_fan_init();
blikvm_int8_t blikvm_fan_start();

#endif