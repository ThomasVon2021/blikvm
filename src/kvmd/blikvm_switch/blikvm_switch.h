#ifndef __BLIKVM_SWITCH_H__
#define __BLIKVM_SWITCH_H__

/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-12-29 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/

#include "common/blikvm_log/blikvm_log.h"

typedef struct
{
    blikvm_int32_t enable;
    blikvm_int8_t device_path[32];
}blikvm_switch_t;

blikvm_int8_t blikvm_switch_init(blikvm_switch_t* config);
blikvm_int8_t blikvm_switch_start();

 #endif