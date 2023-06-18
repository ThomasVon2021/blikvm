/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2023-06-18 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/

#ifndef __BLIKVM_DTC_H__
#define __BLIKVM_DTC_H__

#include "common/blikvm_type.h"


blikvm_int8_t blikvm_dtc_signal_sigint_handler();

blikvm_int8_t blikvm_dtc_signal_sigterm_handler();

#endif