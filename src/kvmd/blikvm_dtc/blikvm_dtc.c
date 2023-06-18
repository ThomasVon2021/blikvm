/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2023-06-18 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blikvm_dtc.h"
#include "../blikvm_oled/blikvm_oled.h"

#include "common/blikvm_log/blikvm_log.h"

#define TAG "DTC"

blikvm_int8_t blikvm_dtc_signal_sigint_handler()
{
    blikvm_oled_info("main exit");
    return 0;
}

blikvm_int8_t blikvm_dtc_signal_sigterm_handler()
{
    blikvm_oled_info("rebooting");
    return 0;
}

