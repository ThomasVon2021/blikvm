/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2023-06-07 | 0.1       | Thmoasvon     |                 create
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <common/blikvm_util/cJSON.h>

#include "kvmd/blikvm_atx/blikvm_atx.h"
#include "blikvm_config.h"

#define TAG "CFG"

static blikvm_config_t g_config = {0};

static char* jsonFromFile(char* filename);

blikvm_int8_t blikvm_config_init()
{
    const char* directory1 = "/mnt/msd/user";
    const char* directory2 = "/mnt/msd/ventoy";

    // Check if the directories already exist
    struct stat dirStat;
    if (stat(directory1, &dirStat) == 0 && S_ISDIR(dirStat.st_mode)) 
    {
        BLILOG_D(TAG,"%s already exists\n", directory1);
    } else 
    {
        // Create directory1 if it doesn't exist
        if (mkdir(directory1, 0777) != 0) 
        {
            BLILOG_E(TAG,"Failed to create %s\n",directory1 );
            return -1;
        } else {
            BLILOG_D(TAG,"%s created\n", directory1);
        }
    }

    if (stat(directory2, &dirStat) == 0 && S_ISDIR(dirStat.st_mode)) {
        BLILOG_D(TAG,"%s already exists\n", directory2);
    } else {
        // Create directory2 if it doesn't exist
        if (mkdir(directory2, 0777) != 0) {
            BLILOG_E(TAG,"Failed to create %s\n",directory2);
            return -1;
        } else {
            BLILOG_D(TAG,"%s created\n", directory2);
        }
    }

    return 0;
}

blikvm_config_t* blikvm_read_config(blikvm_int8_t* file_path)
{
    blikvm_config_t* ret = NULL;
    do
    {
        if(access(file_path, R_OK) != 0)
        {
            BLILOG_E(TAG, "%s config json not exit\n",file_path);
            break;
        }

        char* jsondata = jsonFromFile(file_path);
        cJSON* root=cJSON_Parse(jsondata);
        if (root == NULL)
        {
            BLILOG_E(TAG, "read json data is null\n");
            break;
        }

        const cJSON *atx = NULL;
        atx = cJSON_GetObjectItemCaseSensitive(root, "atx");
        if (cJSON_IsObject(atx))
        {
            const cJSON *power_on_delay = NULL; 
            power_on_delay = cJSON_GetObjectItemCaseSensitive(atx, "power_on_delay");
            if (!cJSON_IsNumber(power_on_delay))
            {
                BLILOG_E(TAG, "power_on_delay is not number\n");
            }
            else
            {
                BLILOG_D(TAG, "power_on_delay:%d\n", power_on_delay->valueint);
                blikvm_atx_set_power_on_delay(power_on_delay->valueint);
            }

            const cJSON *power_off_delay = NULL; 
            power_off_delay = cJSON_GetObjectItemCaseSensitive(atx, "power_off_delay");
            if (!cJSON_IsNumber(power_off_delay))
            {
                BLILOG_E(TAG, "power_off_delay is not number\n");
            }
            else
            {
                BLILOG_D(TAG, "power_off_delay:%d\n",power_off_delay->valueint);
                blikvm_atx_set_power_off_delay(power_off_delay->valueint);
            }
        }

        const cJSON *oled = cJSON_GetObjectItemCaseSensitive(root, "display");
        if (cJSON_IsObject(oled))
        {
            const cJSON *isActive = cJSON_GetObjectItemCaseSensitive(oled, "isActive");
            const cJSON *mode = cJSON_GetObjectItemCaseSensitive(oled, "mode");
            const cJSON *onBootTime = cJSON_GetObjectItemCaseSensitive(oled, "onBootTime");
            const cJSON *cycleInterval = cJSON_GetObjectItemCaseSensitive(oled, "cycleInterval");
            const cJSON *displayTime = cJSON_GetObjectItemCaseSensitive(oled, "displayTime");
            const cJSON *secondIP = cJSON_GetObjectItemCaseSensitive(oled, "secondIP");

            if (cJSON_IsBool(isActive)) {
                g_config.oled.isActive = cJSON_IsTrue(isActive) ? 1 : 0;
                BLILOG_I(TAG, "display isActive: %d\n", g_config.oled.isActive);
            }
            if (cJSON_IsNumber(mode))
            {
                BLILOG_I(TAG, "display mode: %d\n", mode->valueint);
                g_config.oled.mode = mode->valueint;
            }

            if (cJSON_IsNumber(onBootTime))
            {
                BLILOG_I(TAG,"onBootTime: %d\n", onBootTime->valueint);
                g_config.oled.onBootTime = onBootTime->valueint;
            }

            if (cJSON_IsNumber(cycleInterval))
            {
                BLILOG_I(TAG,"cycleInterval: %d\n", cycleInterval->valueint);
                g_config.oled.cycleInterval = cycleInterval->valueint;
            }
            if (cJSON_IsNumber(displayTime))
            {
                BLILOG_I(TAG,"displayTime: %d\n", displayTime->valueint);
                g_config.oled.displayTime = displayTime->valueint;
            }
            if (cJSON_IsString(secondIP) && (secondIP->valuestring != NULL))
            {
                BLILOG_I(TAG,"secondIP: %s\n", secondIP->valuestring);
                strcpy(g_config.oled.secondIP, secondIP->valuestring);
            }
        }
        else
        {
            printf("oled section not found or not an object\n");
        }
       
        const  cJSON *fan = cJSON_GetObjectItemCaseSensitive(root, "fan");
        if (cJSON_IsObject(fan))
        {
            const cJSON *fan_threshold = cJSON_GetObjectItemCaseSensitive(fan, "tempThreshold");
            if (cJSON_IsNumber(fan_threshold))
            {
                g_config.fan.threshold = fan_threshold->valueint;
                BLILOG_I(TAG,"fan_speed: %d\n", g_config.fan.threshold);
            }
        }
        else
        {
            printf("fan section not found or not an object\n");
        }       
        ret = &g_config;

        free(jsondata);
        cJSON_free(root); 
    }while(0>1);
    return ret;
}

char* jsonFromFile(char* filename)
{
    FILE* fp = fopen(filename, "r");
    if (!fp)
    {
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* buf = (char*)calloc(len+1, sizeof(char));
    fread(buf, sizeof(char), len, fp);
    fclose(fp);
    return buf;
}

blikvm_config_t* blikvm_get_config()
{
    return &g_config;
}