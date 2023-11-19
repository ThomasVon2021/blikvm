#ifndef __BLIKVM_ATX_H__
#define __BLIKVM_ATX_H__

/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-09-10 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/

#include "common/blikvm_log/blikvm_log.h"

blikvm_int8_t blikvm_atx_init();
blikvm_int8_t blikvm_atx_start();
blikvm_int8_t blikvm_atx_test();
blikvm_int8_t blikvm_atx_set_power_on_delay(blikvm_int32_t time);
blikvm_int8_t blikvm_atx_set_power_off_delay(blikvm_int32_t time);
#endif