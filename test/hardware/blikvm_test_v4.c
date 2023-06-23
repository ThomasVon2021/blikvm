/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2023-05-28 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#include "GPIO/armbianio.h"
#include "common/blikvm_uart/blikvm_uart.h"
#include "common/blikvm_log/blikvm_log.h"
#include "common/blikvm_util/blikvm_util.h"
#include "kvmd/blikvm_atx/blikvm_atx.h"
#include "blikvm_test.h"

#define SW1 12   //GPIO 257
#define BUZZ 33   //GPIO 271
#define BUFFER_SIZE 1024
#define TAG "TEST"

blikvm_test_result_t g_result = {0};

static blikvm_void_t *blikvm_test_loop(void *_);
static blikvm_int8_t rtc_read_check();

static int uart_init(char* device)
{
    int serial_fd = open(device, O_RDWR | O_NOCTTY  );
    if (serial_fd == -1) {
        perror("Failed to open serial port");
        return -1;
    }

    int set_ret = set_serial_opt(serial_fd, 38400, 8, 'N', 1);  // CHANG BAUD
        if (set_ret == -1) {
            printf("failed to change baut 9600\n");
            return -1;
        }
    return serial_fd;
}

static int usb_init(char* device)
{
    int fd = open(device, O_RDWR| O_NOCTTY  );
    if (fd == -1) 
    {
        BLILOG_E(TAG,"Failed to open device %s\n",device );
        return -1;
    }
    int set_ret = set_serial_opt(fd, 38400, 8, 'N', 1);  // CHANG BAUD
        if (set_ret == -1) {
            printf("failed to change baut 9600\n");
            return -1;
        }
    return fd;
}

static int write_to_fd(const char* string, int serial_fd) 
{
    ssize_t num_bytes = write(serial_fd, string, strlen(string));
    if (num_bytes == -1) {
        perror("Failed to write to serial port");
        close(serial_fd);
        return 0;
    }
    return num_bytes;
}

static char* read_from_fd(int fd) 
{
    char buffer[256] = {0};
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    if (bytesRead == -1) {
        perror("Failed to read from device");
        close(fd);
        return NULL;
    }

    buffer[bytesRead] = '\0';  // Null-terminate the string
    char* result = strdup(buffer);  // Allocate memory and copy the string
    if (result == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }
    return result;
}

static int test_uart_and_usb()
{
    int ret = -1;
    int uart_fd;
    int usb_fd;
    do
    {
        char uart_device[] = "/dev/ttyS0";
        char usb_device[] = "/dev/ttyUSB0";
        char content[] = "blikvmblikvmblikvmblikvmblikvmblikvmblikvm";
        uart_fd = uart_init(uart_device);
        if(uart_fd<= 0)
        {
            printf("uart_fd:%d\n",uart_fd);
            break;
        }

        usb_fd = usb_init(usb_device);

        if( usb_fd <= 0)
        {
            printf("usb_fd:%d \n", usb_fd);
            break;
        }

        if (write_to_fd(content,uart_fd) <= 0 )
        {
            printf("write to uart failed\n");
            break;
        }
        char* buff1 = read_from_fd(usb_fd);
        if(buff1 == NULL )
        {
            printf("read from usb failed\n");
            break;
        }
        free(buff1);
        if (write_to_fd(content,usb_fd) <= 0 )
        {
            printf("write to usb failed\n");
            break;
        }
        char* buff2 = read_from_fd(uart_fd);
        if(buff2 == NULL )
        {
            printf("read from uart failed\n");
            break;
        }
        free(buff2);

        ret = 0;
    }while(0>1);
    if(uart_fd >0 )
    {
        close(uart_fd);
    }
    if(usb_fd >0 )
    {
        close(usb_fd);
    }
    return ret;
}

blikvm_int8_t blikvm_hardware_test()
{
    blikvm_int8_t ret=-1;
    do
    {
        pthread_t blikvm_test_thread;
        AIOAddGPIO(SW1, GPIO_IN);
        AIOAddGPIO(BUZZ, GPIO_OUT);
        ret = pthread_create(&blikvm_test_thread, NULL, blikvm_test_loop, NULL);
    } while (0>1);
    
    return ret;
}

static blikvm_void_t *blikvm_test_loop(void *_)
{
    usleep(5000*1000);

    while(1)
    {
        usleep(1000*1000);

        //test buzz
        if(AIOReadGPIO(SW1) == 1)
        {
            AIOWriteGPIO(BUZZ, GPIO_HIGH);
        }
        else
        {
            AIOWriteGPIO(BUZZ, GPIO_LOW);
        }
        
        //test uart and usb port
        if(!g_result.uart_flag)
        {
            if(test_uart_and_usb() == 0) 
            {
                g_result.uart_flag = 1;
                g_result.usb_flag = 1;
                BLILOG_I(TAG,"test uart and usb ok\n");
            }
            else{
                g_result.code = 0b00000011 | g_result.code;
            }
            
        }
        
        //test rtc 
        if(!g_result.rtc_flag)
        {
            if(rtc_read_check() == 0)
            {
                g_result.rtc_flag = 1;
                BLILOG_I(TAG,"test rtc ok\n");
            }
            else{
                g_result.code = 0b00000100 | g_result.code;
            }
        }

        //test wifi
        if(!g_result.wifi_flag)
        {
            if(isWifiCardAvailable() == 0)
            {
                g_result.wifi_flag = 1;
                BLILOG_I(TAG,"test wifi ok\n");
            }
            else{
                g_result.code = 0b00001000 | g_result.code;
            }
        }

        if(!g_result.atx_flag)
        {
            if(blikvm_atx_test() == 0)
            {
                g_result.atx_flag = 1;
                BLILOG_I(TAG,"test atx ok\n");
            }
            else{
                g_result.code = 0b00010000 | g_result.code;
            }
        }

        if(!g_result.test_flag)
        {
            g_result.test_flag = 1;
        }
    }
    return NULL;
}

static blikvm_int8_t rtc_read_check()
{
    blikvm_int8_t result = -1;
    FILE *fp;
    char command[100];
    char line[100];

    // 检查/dev/rtc1设备是否存在
    if (access("/dev/rtc1", F_OK) == -1) {
        printf("Device /dev/rtc1 does not exist\n");
        return -1;
    }

    // 构造执行命令的字符串
    sprintf(command, "hwclock -f /dev/rtc1 -r");

    // 执行命令并读取输出
    fp = popen(command, "r");
    if (fp == NULL) {
        perror("Failed to execute command");
        return -1;
    }

    // 读取输出并检查时间格式
    if (fgets(line, sizeof(line), fp) != NULL) {
        // 检查行中的时间格式
        if (strstr(line, "-") != NULL && strlen(line) >= 10) {
            result = 0; // 时间格式匹配，设置结果为0
        }
    }

    // 关闭文件指针
    pclose(fp);

    return result;
}

blikvm_test_result_t* blikvm_get_test_result()
{
    return &g_result;
}




