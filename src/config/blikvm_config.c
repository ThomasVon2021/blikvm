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

        // const cJSON *switch_handle = NULL;
        // switch_handle = cJSON_GetObjectItemCaseSensitive(root, "switch_handle");
        // if (cJSON_IsObject(switch_handle))
        // {
        //     const cJSON *switch_enable = cJSON_GetObjectItemCaseSensitive(switch_handle, "switch_enable");
        //     const cJSON *switch_path = cJSON_GetObjectItemCaseSensitive(switch_handle, "switch_path");

        //     if (!cJSON_IsNumber(switch_enable))
        //     {
        //         BLILOG_E(TAG, "switch function is disable\n");
        //     }
        //     else
        //     {
        //         g_config.switch_handle.enable = switch_enable->valueint;
        //         BLILOG_D(TAG, "switch function is enable\n");
        //     } 

        //     if (!cJSON_IsString(switch_path))
        //     {
        //         BLILOG_E(TAG, "switch path is not string\n");
        //     }
        //     else
        //     {
        //         memcpy(g_config.switch_handle.device_path, switch_path->valuestring, strlen(switch_path->valuestring));
        //         BLILOG_D(TAG, "switch device:%s\n",switch_path->valuestring);
        //     }           
        // }

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

        const cJSON *oled = cJSON_GetObjectItemCaseSensitive(root, "oled");
        if (cJSON_IsObject(oled))
        {
            const cJSON *oled_enable = cJSON_GetObjectItemCaseSensitive(oled, "oled_enable");
            const cJSON *restart_show_time = cJSON_GetObjectItemCaseSensitive(oled, "restart_show_time");
            const cJSON *interval_display_time = cJSON_GetObjectItemCaseSensitive(oled, "interval_display_time");

            if (cJSON_IsNumber(oled_enable))
            {
                BLILOG_I(TAG, "oled_enable: %d\n", oled_enable->valueint);
                g_config.oled.oled_enable = oled_enable->valueint;
            }

            if (cJSON_IsNumber(restart_show_time))
            {
                BLILOG_I(TAG,"restart_show_time: %d\n", restart_show_time->valueint);
                g_config.oled.restart_show_time = restart_show_time->valueint;
            }

            if (cJSON_IsNumber(interval_display_time))
            {
                BLILOG_I(TAG,"interval_display_time: %d\n", interval_display_time->valueint);
                g_config.oled.interval_display_time = interval_display_time->valueint;
            }
        }
        else
        {
            printf("oled section not found or not an object\n");
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