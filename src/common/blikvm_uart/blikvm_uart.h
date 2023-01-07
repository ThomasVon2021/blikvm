#ifndef __BLIKVM_UART_H__
#define __BLIKVM_UART_H__

/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-12-29 | 0.1       | Thmoasvon     |                 create
 ******************************************************************************/

#include "common/blikvm_log/blikvm_log.h"

blikvm_int32_t open_serial_dev(const blikvm_int8_t* serial_dev, blikvm_int32_t fd);

 #endif