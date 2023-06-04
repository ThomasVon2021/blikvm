/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2023-06-04 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blikvm_rtc.h"
#include "common/blikvm_log/blikvm_log.h"

#define TAG "RTC"

static blikvm_int8_t i2cdetect_check(blikvm_int8_t* command);

blikvm_int8_t blikvm_rtc_init()
{
    blikvm_int8_t ret = -1;
    do
    {
#ifdef  VER4
        blikvm_int8_t* command = "i2cdetect -y 0";
        if(i2cdetect_check(command) != 0)
        {
            BLILOG_E(TAG,"not detect rtc x51 address on i2c 0\n");
            break;
        }
        blikvm_int8_t* reg_command = {"echo pcf8563 0x51 > /sys/class/i2c-adapter/i2c-0/new_device"};
        popen(reg_command, "r");
        popen("hwclock -f /dev/rtc1 -w", "r");
#endif 
        ret = 0;
    } while (0>1);
    
    return ret;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static blikvm_int8_t i2cdetect_check(blikvm_int8_t* command)
{
    blikvm_int8_t result = -1;
    FILE *fp;
    blikvm_int8_t line[100];

    // 执行命令并读取输出
    fp = popen(command, "r");
    if (fp == NULL) {
        perror("Failed to execute command");
        return -1;
    }

    // 逐行读取输出，并查找是否存在指定的返回值
    while (fgets(line, sizeof(line), fp) != NULL) {
        // 检查行中是否包含指定的返回值
        if (strstr(line, "51") != NULL) {
            result = 0; // 找到指定的返回值，设置结果为0
            break;
        }
    }

    // 关闭文件指针
    pclose(fp);

    return result;
}