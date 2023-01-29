#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <common/blikvm_util/cJSON.h>
#include "blikvm_server.h"
#define TAG "MAIN"


static char* config_json_path= (char*)"/usr/bin/package.json";
static char* jsonFromFile(char* filename);
static blikvm_int8_t blikvm_parse_config(blikvm_int8_t* file_path);
static blikvm_config_t config;

int main(int argc, char *argv[])
{
    
    blikvm_uint8_t log_path[] = "/opt/bin/blilog";
    config.log.enable[0] = 1;
    config.log.level = 4U;
    config.log.channel = BLI_LOG_CHANNEL_CONSOLE | BLI_LOG_CHANNEL_FILE;
    memcpy(config.log.log_out.file.path, log_path, strnlen((blikvm_int8_t *)log_path, 256));
    config.log.log_out.file.max_size[0] =  5 * 1024U;
    config.oled_type = OLED_SSD1306_128_64;
    blikvm_parse_config(config_json_path);
    blikvm_init(&config);
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
            BLILOG_E(TAG,"%s config json not exit\n",file_path);
            break;
        }
        const cJSON *oled_type = NULL;
        char* jsondata = jsonFromFile(file_path);
        printf("%s\n", jsondata);
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
            break;
        }
        printf("oled type:%d\n",oled_type->valueint);
        //TODO 
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