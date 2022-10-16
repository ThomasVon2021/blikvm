#ifndef __BLIKVM_SERVER_H__
#define __BLIKVM_SERVER_H__

/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-09-02 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/

#include "common/blikvm_type.h"
#include "common/blikvm_log/blikvm_log.h"
#include "kvmd/blikvm_oled/blikvm_oled.h"

typedef struct
{
    blikvm_log_t log;
    blikvm_oled_type_e oled_type;
}blikvm_config_t;

blikvm_int8_t blikvm_init( blikvm_config_t *config);
blikvm_int8_t blikvm_start();

#endif