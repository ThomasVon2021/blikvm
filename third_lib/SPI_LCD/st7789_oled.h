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

blikvm_int8_t oled_240_240_run();

blikvm_int8_t blikvm_oled_stop();

blikvm_int8_t blikvm_backlight_close();

blikvm_int8_t blikvm_backlight_open();

blikvm_int8_t oled_extra_show(blikvm_int8_t* buff);

#endif