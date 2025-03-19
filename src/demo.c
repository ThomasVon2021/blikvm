#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "blikvm_server.h"
#include "config/blikvm_config.h"
#include "kvmd/blikvm_atx/blikvm_atx.h"
#define TAG "MAIN"


//static char* config_json_path= (char*)"/mnt/blikvm/web_src/web_server/config/app.json";
static char* config_json_path= (char*)"/mnt/exec/release/config/app.json";
static blikvm_log_t g_log_config={0};
static blikvm_config_t* g_config = NULL;

int main(int argc, char *argv[])
{
    
    blikvm_uint8_t log_path[] = "/tmp/";
    g_log_config.enable[0] = 1;
    g_log_config.level = 4U;
    g_log_config.channel = BLI_LOG_CHANNEL_CONSOLE | BLI_LOG_CHANNEL_FILE;
    memcpy(g_log_config.log_out.file.path, log_path, strlen((char *)log_path) + 1);
    g_log_config.log_out.file.max_size[0] =  5 * 1024U;
    setvbuf(stdout, NULL, _IOLBF, 0);
    //1. init log module
    blikvm_log_init(&g_log_config);

    //2. read config json
    g_config = blikvm_read_config(config_json_path);
    if( g_config == NULL)
    {
        BLILOG_E(TAG,"read config json failed\n");
    }
    blikvm_init(g_config);
    blikvm_start(g_config);
    while (1)
    {
        usleep(10000*1000);
    }
    
    return 0;
}
