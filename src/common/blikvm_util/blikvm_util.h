#ifndef __BLIKVM_UTIL_H__
#define __BLIKVM_UTIL_H__

/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-01-07 | 0.1       | Thmoasvon     |                 create
 ******************************************************************************/

#include "common/blikvm_log/blikvm_log.h"

blikvm_int32_t execmd(blikvm_int8_t* cmd, blikvm_int8_t* result);
blikvm_void_t split(blikvm_int8_t *src,const blikvm_int8_t *separator,blikvm_int8_t **dest,blikvm_int32_t *num);

/**
 * @brief : get os's ms
 * @param[in]: none
 *
 * @return: the os's utc milliseconds since the Epoch
 */
blikvm_uint64_t blikvm_get_utc_ms(void);

#endif