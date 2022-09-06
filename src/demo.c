#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "blikvm_server.h"
#define TAG "MAIN"

int main(int argc, char *argv[])
{
    blikvm_config_t config;
    blikvm_uint8_t log_path[] = "./blilog";
    config.log.enable[0] = 1;
    config.log.level = 4U;
    config.log.channel = 2U;
    memcpy(config.log.log_out.file.path, log_path, strnlen((blikvm_int8_t *)log_path, 256));
    config.log.log_out.file.max_size[0] =  5 * 1024U;
    
    blikvm_init(&config);

    while (1)
    {
        usleep(100*1000);
    }
    
    return 0;
}