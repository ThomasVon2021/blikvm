#ifndef __BLIKVM_OLED_H__
#define __BLIKVM_OLED_H__

/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-09-10 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/

#include "common/blikvm_type.h"

typedef enum
{
    OLED_SSD1306_128_32 = 0,
    OLED_SSD1306_128_64 = 1,
}blikvm_oled_type_e;

blikvm_int8_t blikvm_oled_init(blikvm_oled_type_e type);

blikvm_int8_t blikvm_oled_start();

 #endif