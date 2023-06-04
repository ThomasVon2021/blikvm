/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2023-05-28 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/

#ifndef __BLIKVM_TEST_H__
#define __BLIKVM_TEST_H__

#include "common/blikvm_type.h"


typedef struct
{
    blikvm_int8_t test_flag;
    blikvm_int8_t uart_flag; 
    blikvm_int8_t usb_flag;
    blikvm_int8_t rtc_flag;
    blikvm_int8_t wifi_flag;
    blikvm_int8_t atx_flag;
    // 0 is ok, 1 is error, like code=0, all ok, like code=3, usb and uart is eroor
    //uart: first bit, usb: second bit, rtc: third bit, wifi: fourth bit, atx: fifth bit
    blikvm_int8_t code; 
}blikvm_test_result_t;

blikvm_int8_t blikvm_hardware_test();

blikvm_test_result_t* blikvm_get_test_result();

#endif