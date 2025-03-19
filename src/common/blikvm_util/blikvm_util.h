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

typedef enum
{
    UNKONW_BOARD = 0,
    PI4B_BOARD = 1,
    CM4_BOARD = 2,
    H616_BOARD = 3,
}blikvm_board_type_e;

/**
 * @brief : get os's ms
 * @param[in]: none
 *
 * @return: the os's utc milliseconds since the Epoch
 */
blikvm_uint64_t blikvm_get_utc_ms(void);
float GetCPULoad();
int GetMemUsage(char* mem);
int GetCPUTemp() ;
int GetIP(char* ip);
int GetSpecificIP(const char* interface, char* ip);
char* GetUptime(); 
int GetMemUsageShort(char* mem);
blikvm_board_type_e blikvm_get_board_type();
blikvm_int8_t isWifiCardAvailable();

/**
 * @brief : get os's up time 
 * @return: the os's up  since the Epoch, unit:sec
 */
blikvm_int32_t skdy_get_int_uptime();

#endif