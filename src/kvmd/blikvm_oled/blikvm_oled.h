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

#define MIN_START_SHOW_TIME 5  //Minimum continuous on time of startup screen

typedef enum
{
    OLED_SSD1306_128_32 = 0,
    OLED_SSD1306_128_64 = 1,
    OLED_ST7789_240_240 = 2,
}blikvm_oled_type_e;

typedef struct
{
    blikvm_int8_t      oled_type;
    blikvm_int32_t     oled_always_enable;
    blikvm_int32_t     display_enable;
    blikvm_int32_t     restart_show_time;  // unit: min
    blikvm_int32_t     interval_display_time; //unit: min
}blikvm_oled_config_t;

blikvm_int8_t blikvm_oled_init(blikvm_oled_config_t* config);

blikvm_int8_t blikvm_oled_start();

blikvm_int8_t blikvm_oled_stop();

blikvm_void_t blikvm_oled_set_display_enable( blikvm_int32_t value);

#endif