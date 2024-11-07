/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2023-05-28 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/

#ifndef __BLIKVM_ST7789_OLED_H__
#define __BLIKVM_ST7789_OLED_H__

#include "common/blikvm_type.h"

blikvm_int32_t oled_240_240_init();

blikvm_int32_t oled_240_240_show(blikvm_int32_t diff);

blikvm_int32_t blikvm_backlight_close();

blikvm_int32_t blikvm_backlight_open();

#endif