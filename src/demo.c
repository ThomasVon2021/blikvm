#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <common/blikvm_util/cJSON.h>
#include "blikvm_server.h"
#include "kvmd/blikvm_atx/blikvm_atx.h"
#define TAG "MAIN"


static char* config_json_path= (char*)"/usr/bin/blikvm/package.json";
static char* jsonFromFile(char* filename);
static blikvm_int8_t blikvm_parse_config(blikvm_int8_t* file_path);
static blikvm_config_t g_config;

int main(int argc, char *argv[])
{
    
    blikvm_uint8_t log_path[] = "/tmp/";
    g_config.log.enable[0] = 1;
    g_config.log.level = 4U;
    g_config.log.channel = BLI_LOG_CHANNEL_CONSOLE | BLI_LOG_CHANNEL_FILE;
    memcpy(g_config.log.log_out.file.path, log_path, strnlen((blikvm_int8_t *)log_path, 256));
    g_config.log.log_out.file.max_size[0] =  5 * 1024U;
    g_config.oled_type = OLED_SSD1306_128_64;
    blikvm_parse_config(config_json_path);
    blikvm_init(&g_config);
    blikvm_start();
    while (1)
    {
        usleep(10000*1000);
    }
    
    return 0;
}

blikvm_int8_t blikvm_parse_config(blikvm_int8_t* file_path)
{
    blikvm_int8_t ret = -1;
    do
    {
        if(access(file_path, R_OK) != 0)
        {
            printf("%s config json not exit\n",file_path);
            break;
        }
        const cJSON *oled_type = NULL;
        const cJSON *switch_device = NULL;
        char* jsondata = jsonFromFile(file_path);
        cJSON* root=cJSON_Parse(jsondata);
        if (root == NULL)
        {
            printf("read json data is null\n");
            break;
        }
        oled_type = cJSON_GetObjectItemCaseSensitive(root, "oled_type");
        if (!cJSON_IsNumber(oled_type))
        {
            printf("oled_type is not number\n");
        }
        else
        {
            printf("oled type:%d\n",oled_type->valueint);
        }
        
        switch_device = cJSON_GetObjectItemCaseSensitive(root, "switch_device");
        if (!cJSON_IsString(switch_device))
        {
            printf("switch_device is not string\n");
        }
        else
        {
            memcpy(g_config.switch_device,switch_device->valuestring,strlen(switch_device->valuestring));
            printf("switch device:%s\n",switch_device->valuestring);
        }

        const cJSON *power_on_dalay = NULL; 
        power_on_dalay = cJSON_GetObjectItemCaseSensitive(root, "power_on_dalay");
        if (!cJSON_IsNumber(power_on_dalay))
        {
            printf("power_on_dalay is not number\n");
        }
        else
        {
            printf("power_on_dalay:%d\n",power_on_dalay->valueint);
            blikvm_atx_set_power_on_dalay(power_on_dalay->valueint);
        }

        const cJSON *power_off_dalay = NULL; 
        power_off_dalay = cJSON_GetObjectItemCaseSensitive(root, "power_off_dalay");
        if (!cJSON_IsNumber(power_off_dalay))
        {
            printf("power_off_dalay is not number\n");
        }
        else
        {
            printf("power_off_dalay:%d\n",power_off_dalay->valueint);
            blikvm_atx_set_power_off_dalay(power_off_dalay->valueint);
        }

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