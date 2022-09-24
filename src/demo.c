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
    config.log.channel = BLI_LOG_CHANNEL_CONSOLE | BLI_LOG_CHANNEL_FILE;
    memcpy(config.log.log_out.file.path, log_path, strnlen((blikvm_int8_t *)log_path, 256));
    config.log.log_out.file.max_size[0] =  5 * 1024U;
    config.oled_type = OLED_SSD1306_128_64;
    
    blikvm_init(&config);
    blikvm_start();
    while (1)
    {
        usleep(100*1000);
    }
    
    return 0;
}